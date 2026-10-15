#include "services/palace_service.hpp"

#include <algorithm>

using namespace std;

namespace tdp {

OpResult PalaceService::addCustomer(const string& name, const string& phone, const string& email,
                                    const string& address, const string& notes) {
    auto r = require(Action::ManageCustomers);
    if (!r.ok) return r;
    if (trim(name).empty()) return OpResult::fail("Tên khách hàng là bắt buộc.");
    if (!looksLikePhone(trim(phone))) return OpResult::fail("Số điện thoại không hợp lệ (9-12 chữ số).");
    if (!looksLikeEmail(trim(email))) return OpResult::fail("Email không đúng định dạng.");
    if (!uniquePhone(trim(phone), "")) return OpResult::fail("Số điện thoại đã tồn tại.");
    string id = nextId("CUS-", customerIds());
    mut().customers.push_back(Customer(id, trim(name), trim(phone), trim(email), trim(address), trim(notes),
                                       DateTime::now()));
    return persist("Đã thêm khách hàng " + id, id);
}

OpResult PalaceService::updateCustomer(const string& id, const string& name, const string& phone,
                                       const string& email, const string& address, const string& notes) {
    auto r = require(Action::ManageCustomers);
    if (!r.ok) return r;
    auto* c = findById(mut().customers, id);
    if (!c) return OpResult::fail("Không tìm thấy khách hàng.");
    if (trim(name).empty()) return OpResult::fail("Tên khách hàng là bắt buộc.");
    if (!looksLikePhone(trim(phone))) return OpResult::fail("Số điện thoại không hợp lệ.");
    if (!looksLikeEmail(trim(email))) return OpResult::fail("Email không đúng định dạng.");
    if (!uniquePhone(trim(phone), id)) return OpResult::fail("Số điện thoại đã tồn tại.");
    c->setName(trim(name));
    c->setPhone(trim(phone));
    c->setEmail(trim(email));
    c->setAddress(trim(address));
    c->setNotes(trim(notes));
    return persist("Đã cập nhật khách hàng.", id);
}

OpResult PalaceService::deleteCustomer(const string& id) {
    auto r = require(Action::ManageCustomers);
    if (!r.ok) return r;
    if (!findById(mut().customers, id)) return OpResult::fail("Không tìm thấy khách hàng.");
    for (const auto& q : data().requests)
        if (q.customerId() == id) return OpResult::fail("Khách hàng đã có yêu cầu, không thể xóa.");
    for (const auto& b : data().bookings)
        if (b.customerId() == id) return OpResult::fail("Khách hàng đã có booking, không thể xóa.");
    for (const auto& u : data().users)
        if (u.customerId() == id) return OpResult::fail("Khách hàng đang gắn tài khoản, không thể xóa.");
    auto& items = mut().customers;
    items.erase(remove_if(items.begin(), items.end(),
                          [&](const Customer& c) { return c.id() == id; }),
                items.end());
    return persist("Đã xóa khách hàng.", id);
}

vector<Customer> PalaceService::searchCustomers(const string& keyword) const {
    vector<Customer> out;
    for (const auto& c : data().customers) {
        if (keyword.empty() || containsIgnoreCase(c.id(), keyword) || containsIgnoreCase(c.name(), keyword) ||
            containsIgnoreCase(c.phone(), keyword))
            out.push_back(c);
    }
    return out;
}

int PalaceService::resolveQuantity(const ServiceItem& svc, int guestCount, int qty) const {
    if (qty > 0) return qty;
    if (svc.unit().find("/bàn") != string::npos) return (guestCount + 9) / 10;
    if (svc.unit().find("/khách") != string::npos) return guestCount;
    return 1;
}

OpResult PalaceService::buildLines(int guestCount, const vector<ServiceLineInput>& in,
                                   vector<RequestServiceLine>& out) const {
    out.clear();
    for (const auto& li : in) {
        const ServiceItem* svc = findService(li.serviceId);
        if (!svc) svc = findServiceByCode(li.serviceId);
        if (!svc) return OpResult::fail("Dịch vụ không tồn tại: " + li.serviceId);
        if (!svc->active()) return OpResult::fail("Dịch vụ đã ngừng cung cấp: " + svc->name());
        out.push_back(RequestServiceLine(svc->id(), svc->name(), svc->unit(),
                                         resolveQuantity(*svc, guestCount, li.quantity),
                                         svc->unitPrice()));
    }
    return OpResult::success();
}

int PalaceService::maxHallCapacity() const {
    int m = 0;
    for (const auto& h : data().halls) m = max(m, h.capacity());
    return m > 0 ? m : 800;
}

static bool validRequestWindow(DateTime start, DateTime end, int guestCount, int maxCap, string& err) {
    if (!start.valid() || !end.valid()) {
        err = "Ngày giờ không hợp lệ.";
        return false;
    }
    if (!(start < end)) {
        err = "Thời gian bắt đầu phải sớm hơn thời gian kết thúc.";
        return false;
    }
    if (start < DateTime::now().plusHours(-1)) {
        err = "Ngày diễn ra phải trong tương lai (từ ngày mai trở đi).";
        return false;
    }
    if (guestCount < 10 || guestCount > maxCap) {
        err = "Số lượng khách từ 10 đến " + to_string(maxCap) + ".";
        return false;
    }
    return true;
}

OpResult PalaceService::addRequest(const string& customerId, EventType type, const string& title,
                                   const string& customerName, const string& customerPhone,
                                   const string& hallId, DateTime start, DateTime end, int guestCount,
                                   const string& notes, const vector<ServiceLineInput>& services) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    // Khach hang chi tao yeu cau cho chinh minh; ten/SDT tu dien tu ho so (Form 1).
    const User* u = currentUser();
    string cid = customerId;
    string cname = trim(customerName), cphone = trim(customerPhone);
    if (u && u->role() == Role::Customer) {
        if (u->customerId().empty()) return OpResult::fail("Tài khoản chưa gắn hồ sơ khách hàng.");
        cid = u->customerId();
        if (const Customer* self = findCustomer(cid)) {
            if (cname.empty()) cname = self->name();
            if (cphone.empty()) cphone = self->phone();
        }
    }
    if (!findCustomer(cid)) return OpResult::fail("Khách hàng không tồn tại.");
    if (trim(title).empty()) return OpResult::fail("Tên sự kiện là bắt buộc.");
    if (cname.empty()) return OpResult::fail("Họ tên khách hàng là bắt buộc.");
    if (!looksLikePhone(cphone)) return OpResult::fail("Số điện thoại không hợp lệ.");
    if (!hallId.empty() && !findHall(hallId)) return OpResult::fail("Sảnh mong muốn không tồn tại.");
    string err;
    if (!validRequestWindow(start, end, guestCount, maxHallCapacity(), err)) return OpResult::fail(err);
    vector<RequestServiceLine> lines;
    OpResult rl = buildLines(guestCount, services, lines);
    if (!rl.ok) return rl;
    // Khach sua ten/SDT tren form -> dong bo nguoc ve ho so cua chinh ho.
    if (u && u->role() == Role::Customer) {
        if (auto* self = findById(mut().customers, cid)) {
            if (cname != self->name() || cphone != self->phone()) {
                if (!uniquePhone(cphone, cid))
                    return OpResult::fail("Số điện thoại đã được dùng cho hồ sơ khác.");
                self->setName(cname);
                self->setPhone(cphone);
            }
        }
    }
    vector<string> ids;
    for (const auto& x : data().requests) ids.push_back(x.id());
    string id = nextId("REQ-", ids);
    EventRequest req(id, cid, type, trim(title), cname, cphone, hallId, start, end, guestCount,
                     trim(notes), RequestStatus::Cho);
    req.lines() = move(lines);
    req.setCreatedAt(DateTime::now());
    mut().requests.push_back(move(req));
    return persist("Đã gửi yêu cầu " + id + ". Chi phí dự kiến: " + formatMoney(estimateRequest(*findRequest(id))),
                   id);
}

OpResult PalaceService::updateRequest(const string& id, EventType type, const string& title,
                                      const string& customerName, const string& customerPhone,
                                      const string& hallId, DateTime start, DateTime end, int guestCount,
                                      const string& notes, const vector<ServiceLineInput>& services) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, id);
    if (!req) return OpResult::fail("Không tìm thấy yêu cầu.");
    const User* actor = currentUser();
    if (actor && actor->role() == Role::Customer && req->customerId() != actor->customerId())
        return OpResult::fail("Bạn chỉ sửa yêu cầu của mình.");
    if (req->status() == RequestStatus::DaChot || req->status() == RequestStatus::BiHuy)
        return OpResult::fail("Yêu cầu đã chốt/hủy, không cập nhật.");
    if (trim(title).empty()) return OpResult::fail("Tên sự kiện là bắt buộc.");
    if (!looksLikePhone(trim(customerPhone))) return OpResult::fail("Số điện thoại không hợp lệ.");
    if (!hallId.empty() && !findHall(hallId)) return OpResult::fail("Sảnh mong muốn không tồn tại.");
    string err;
    if (!validRequestWindow(start, end, guestCount, maxHallCapacity(), err)) return OpResult::fail(err);
    vector<RequestServiceLine> lines;
    OpResult rl = buildLines(guestCount, services, lines);
    if (!rl.ok) return rl;
    req->setType(type);
    req->setTitle(trim(title));
    req->setCustomerName(trim(customerName));
    req->setCustomerPhone(trim(customerPhone));
    req->setHallId(hallId);
    req->setStart(start);
    req->setEnd(end);
    req->setGuestCount(guestCount);
    req->setNotes(trim(notes));
    req->lines() = move(lines);
    return persist("Đã cập nhật yêu cầu " + id + ". Chi phí dự kiến: " + formatMoney(estimateRequest(*req)),
                   id);
}

OpResult PalaceService::updateRequestStatus(const string& id, RequestStatus status) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, id);
    if (!req) return OpResult::fail("Không tìm thấy yêu cầu.");
    const User* u = currentUser();
    // Khach hang chi duoc huy yeu cau cua minh.
    if (u && u->role() == Role::Customer) {
        if (req->customerId() != u->customerId()) return OpResult::fail("Bạn chỉ xử lý yêu cầu của mình.");
        if (status != RequestStatus::BiHuy) return OpResult::fail("Khách hàng chỉ được hủy yêu cầu.");
        if (req->status() == RequestStatus::DaChot || req->status() == RequestStatus::BiHuy)
            return OpResult::fail("Yêu cầu đã kết thúc, không thể hủy.");
        req->setStatus(RequestStatus::BiHuy);
        return persist("Đã hủy yêu cầu " + id, id);
    }
    if (req->status() == RequestStatus::DaChot || req->status() == RequestStatus::BiHuy)
        return OpResult::fail("Yêu cầu đã chốt/hủy, không đổi trạng thái.");
    if (status == RequestStatus::Cho || status == RequestStatus::DangTuVan) {
        req->setStatus(status);
        return persist("Đã cập nhật trạng thái yêu cầu.", id);
    }
    if (status == RequestStatus::BiHuy) {
        req->setStatus(RequestStatus::BiHuy);
        return persist("Đã hủy yêu cầu.", id);
    }
    if (status == RequestStatus::HoanThanh || status == RequestStatus::DaChot) {
        // Yeu cau Hoan thanh -> tu dong sang tab Booking (dac ta B.2).
        string bookingId;
        const Hall* hall = req->hallId().empty() ? nullptr : findHall(req->hallId());
        if (!hall) {
            auto avail = findAvailableHalls(req->start(), req->end(), req->guestCount(), "");
            if (avail.empty()) return OpResult::fail("Không còn sảnh trống, cần tư vấn lại với khách.");
            hall = findHall(avail.front().id());
        }
        OpResult br = applyBookingFromRequest(req->id(), hall->id(), req->start(), req->end(),
                                              req->guestCount(), 0, 0, bookingId);
        if (!br.ok) return br;
        return persist("Yêu cầu hoàn thành, đã chuyển sang Booking " + bookingId + ".", bookingId);
    }
    return OpResult::fail("Trạng thái không hợp lệ.");
}

OpResult PalaceService::cancelOwnRequest(const string& id) {
    return updateRequestStatus(id, RequestStatus::BiHuy);
}

OpResult PalaceService::deleteRequest(const string& id) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, id);
    if (!req) return OpResult::fail("Không tìm thấy yêu cầu.");
    const User* u = currentUser();
    if (u && u->role() == Role::Customer && req->customerId() != u->customerId())
        return OpResult::fail("Bạn chỉ xóa yêu cầu của mình.");
    if (req->status() == RequestStatus::DaChot)
        return OpResult::fail("Yêu cầu đã chốt booking, không thể xóa (hãy Hủy booking).");
    for (const auto& b : data().bookings)
        if (b.requestId() == id) return OpResult::fail("Yêu cầu đã có booking, không thể xóa.");
    auto& items = mut().requests;
    items.erase(remove_if(items.begin(), items.end(),
                          [&](const EventRequest& x) { return x.id() == id; }),
                items.end());
    return persist("Đã xóa yêu cầu.", id);
}

int64_t PalaceService::estimateRequest(const EventRequest& req) const {
    int64_t total = req.servicesTotal();
    if (!req.hallId().empty()) {
        if (const Hall* h = findHall(req.hallId())) total += h->rentalPrice();
    }
    return total;
}

vector<EventRequest> PalaceService::requestHistory(size_t limit) const {
    vector<EventRequest> out;
    for (const auto& r : data().requests) {
        if (r.status() == RequestStatus::BiHuy || r.status() == RequestStatus::DaChot ||
            r.status() == RequestStatus::HoanThanh)
            out.push_back(r);
    }
    sort(out.begin(), out.end(),
         [](const EventRequest& a, const EventRequest& b) { return b.createdAt() < a.createdAt(); });
    if (out.size() > limit) out.resize(limit);
    return out;
}

size_t PalaceService::countNewRequests() const {
    size_t n = 0;
    for (const auto& r : data().requests)
        if (r.status() == RequestStatus::Cho) ++n;
    return n;
}

vector<EventRequest> PalaceService::requestsForCustomer(const string& customerId) const {
    vector<EventRequest> out;
    for (const auto& r : data().requests)
        if (r.customerId() == customerId) out.push_back(r);
    sort(out.begin(), out.end(),
         [](const EventRequest& a, const EventRequest& b) { return b.createdAt() < a.createdAt(); });
    return out;
}

}  // namespace tdp
