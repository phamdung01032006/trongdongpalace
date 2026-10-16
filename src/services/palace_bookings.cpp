#include "services/palace_service.hpp"

#include <algorithm>

using namespace std;

namespace tdp {

int64_t discountLimitFor(int64_t total) { return total * kDiscountApprovalPercent / 100; }

OpResult PalaceService::addHall(const string& name, int capacity, int64_t rentalPrice) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (trim(name).empty()) return OpResult::fail("Tên sảnh là bắt buộc.");
    if (capacity <= 0) return OpResult::fail("Sức chứa phải lớn hơn 0.");
    if (rentalPrice < 0) return OpResult::fail("Giá thuê không âm.");
    vector<string> ids;
    for (const auto& x : data().halls) ids.push_back(x.id());
    string id = nextId("HAL-", ids);
    mut().halls.push_back(Hall(id, trim(name), capacity, rentalPrice, HallStatus::KhaDung));
    return persist("Đã thêm sảnh " + id + ".", id);
}

OpResult PalaceService::updateHall(const string& id, const string& name, int capacity, int64_t rentalPrice,
                                   HallStatus status) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* h = findById(mut().halls, id);
    if (!h) return OpResult::fail("Không tìm thấy sảnh.");
    if (trim(name).empty() || capacity <= 0) return OpResult::fail("Tên sảnh và sức chứa không hợp lệ.");
    if (rentalPrice < 0) return OpResult::fail("Giá thuê không âm.");
    h->setName(trim(name));
    h->setCapacity(capacity);
    h->setRentalPrice(rentalPrice);
    h->setStatus(status);
    return persist("Đã cập nhật sảnh.", id);
}

OpResult PalaceService::deleteHall(const string& id) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (!findById(mut().halls, id)) return OpResult::fail("Không tìm thấy sảnh.");
    for (const auto& b : data().bookings)
        if (b.hallId() == id) return OpResult::fail("Sảnh đã có booking, không thể xóa.");
    for (const auto& q : data().requests)
        if (q.hallId() == id) return OpResult::fail("Sảnh đang được yêu cầu chọn, không thể xóa.");
    auto& items = mut().halls;
    items.erase(remove_if(items.begin(), items.end(), [&](const Hall& x) { return x.id() == id; }),
                items.end());
    return persist("Đã xóa sảnh.", id);
}

OpResult PalaceService::addService(const string& code, const string& name, const string& unit, int64_t price,
                                   const string& category) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (trim(name).empty() || price < 0) return OpResult::fail("Tên dịch vụ bắt buộc, đơn giá không âm.");
    vector<string> ids;
    for (const auto& x : data().services) ids.push_back(x.id());
    string id = nextId("SVC-", ids);
    mut().services.push_back(ServiceItem(id, trim(code).empty() ? id : trim(code), trim(name),
                                         trim(unit).empty() ? "VNĐ/gói" : trim(unit), price,
                                         trim(category).empty() ? "Khác" : trim(category), true));
    return persist("Đã thêm dịch vụ.", id);
}

OpResult PalaceService::updateService(const string& id, const string& code, const string& name,
                                      const string& unit, int64_t price, const string& category,
                                      bool active) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* s = findById(mut().services, id);
    if (!s) return OpResult::fail("Không tìm thấy dịch vụ.");
    if (trim(name).empty() || price < 0) return OpResult::fail("Dữ liệu dịch vụ không hợp lệ.");
    s->setCode(trim(code).empty() ? s->id() : trim(code));
    s->setName(trim(name));
    s->setUnit(trim(unit));
    s->setUnitPrice(price);
    s->setCategory(trim(category));
    s->setActive(active);
    return persist("Đã cập nhật dịch vụ (đơn đã chốt vẫn giữ đơn giá cũ).", id);
}

OpResult PalaceService::deleteService(const string& id) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    const ServiceItem* target = findService(id);
    if (!target) target = findServiceByCode(id);
    if (!target) return OpResult::fail("Không tìm thấy dịch vụ.");
    const string realId = target->id();
    for (const auto& q : data().requests) {
        for (const auto& l : q.lines()) {
            if (l.serviceId() == realId)
                return OpResult::fail("Dịch vụ đang dùng trong yêu cầu " + q.id() + ", không thể xóa.");
        }
    }
    auto& items = mut().services;
    items.erase(remove_if(items.begin(), items.end(),
                          [&](const ServiceItem& x) { return x.id() == realId; }),
                items.end());
    return persist("Đã xóa dịch vụ.", realId);
}

bool PalaceService::hallAvailable(const string& hallId, DateTime start, DateTime end,
                                  const string& ignoreBookingId, string& reason) const {
    const Hall* hall = findHall(hallId);
    if (!hall) {
        reason = "Sảnh không tồn tại.";
        return false;
    }
    if (hall->status() != HallStatus::KhaDung) {
        reason = "Sảnh '" + hall->name() + "' hiện ở trạng thái " + toString(hall->status()) + ".";
        return false;
    }
    if (!(start < end)) {
        reason = "Thời gian bắt đầu phải sớm hơn kết thúc.";
        return false;
    }
    for (const auto& b : data().bookings) {
        if (!b.occupiesHall()) continue;
        if (b.id() == ignoreBookingId) continue;
        if (b.hallId() != hallId) continue;
        if (timeOverlaps(start, end, b.start(), b.end())) {
            reason = "Sảnh bị trùng lịch với booking " + b.id() + " (" + b.start().toString() + " - " +
                     b.end().toString() + ").";
            return false;
        }
    }
    return true;
}

vector<Hall> PalaceService::findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                              const string& ignoreBookingId) const {
    vector<Hall> out;
    string reason;
    for (const auto& h : data().halls) {
        if (guestCount > h.capacity()) continue;
        if (hallAvailable(h.id(), start, end, ignoreBookingId, reason)) out.push_back(h);
    }
    return out;
}

vector<Hall> PalaceService::filterHalls(DateTime start, DateTime end, int guestCount,
                                          int64_t maxPrice, const string& ignoreBookingId) const {
    vector<Hall> out;
    for (const Hall& h : findAvailableHalls(start, end, guestCount, ignoreBookingId)) {
        if (maxPrice > 0 && h.rentalPrice() > maxPrice) continue;
        out.push_back(h);
    }
    return out;
}

OpResult PalaceService::applyBookingFromRequest(const string& requestId, const string& hallId, DateTime start,
                                                DateTime end, int guestCount, int discountPercent,
                                                int64_t discountAmount, string& bookingId) {
    auto* req = findById(mut().requests, requestId);
    if (!req) return OpResult::fail("Yêu cầu sự kiện không tồn tại.");
    if (req->status() == RequestStatus::BiHuy || req->status() == RequestStatus::DaChot)
        return OpResult::fail("Yêu cầu đã hủy/chốt, không tạo booking.");
    const Hall* hall = findHall(hallId);
    if (!hall) return OpResult::fail("Sảnh chốt không tồn tại.");
    if (guestCount <= 0) return OpResult::fail("Số khách phải lớn hơn 0.");
    if (guestCount > hall->capacity()) return OpResult::fail("Số khách vượt sức chứa sảnh.");
    string reason;
    if (!hallAvailable(hallId, start, end, "", reason)) return OpResult::fail(reason);

    int64_t total = hall->rentalPrice() + req->servicesTotal();
    bool needsApproval = false;
    const User* actor = currentUser();
    bool isManager = actor && actor->role() == Role::Manager;
    if (discountPercent > 0 || discountAmount > 0) {
        if (discountPercent < 0 || discountAmount < 0) return OpResult::fail("Chiết khấu không âm.");
        if (discountPercent > 100) return OpResult::fail("Chiết khấu % tối đa 100.");
        if (discountAmount > total) return OpResult::fail("Giảm giá vượt tổng chi phí.");
        if ((discountPercent > kDiscountApprovalPercent ||
             discountAmount > discountLimitFor(total)) &&
            !isManager) {
            needsApproval = true;
        }
    }

    vector<string> ids;
    for (const auto& x : data().bookings) ids.push_back(x.id());
    string id = nextId("BKG-", ids);
    Booking b(id, requestId, req->customerId(), hallId, start, end, guestCount, total, BookingStatus::ChoCoc);
    DateTime now = DateTime::now();
    b.setCreatedAt(now);
    b.setHoldUntil(now.plusHours(kHoldHours));
    b.setSalesUsername(actor ? actor->username() : "");
    b.setDiscount(discountPercent, discountAmount, needsApproval);
    mut().bookings.push_back(move(b));
    req->setStatus(RequestStatus::DaChot);

    // Khoan dat coc tu dong tao, gia tri de trong (dac ta B.3).
    // Su kien se tu tao khi thu coc xong (khong tao o day).
    vector<string> pids;
    for (const auto& x : data().payments) pids.push_back(x.id());
    string pid = nextId("PAY-", pids);
    mut().payments.push_back(Payment(pid, id, PaymentType::DatCoc1, 0, now, PaymentMethod::ChuyenKhoan,
                                     PaymentStatus::ChuaThanhToan, "Tiền đặt cọc giữ chỗ",
                                     actor ? actor->username() : ""));
    bookingId = id;
    if (needsApproval)
        return persist("Đã tạo booking " + id + " (giữ chỗ " + to_string(kHoldHours) +
                           "h). Chiết khấu vượt " + to_string(kDiscountApprovalPercent) +
                           "% cần Quản lý duyệt.",
                       id);
    return persist("Đã tạo booking " + id + " từ yêu cầu " + requestId + " (giữ chỗ " +
                       to_string(kHoldHours) + "h chờ đặt cọc).",
                   id);
}

OpResult PalaceService::createBooking(const string& requestId, const string& hallId, DateTime start,
                                      DateTime end, int guestCount, int discountPercent,
                                      int64_t discountAmount) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    string bookingId;
    OpResult br = applyBookingFromRequest(requestId, hallId, start, end, guestCount, discountPercent,
                                          discountAmount, bookingId);
    if (!br.ok) return br;
    return br;
}

OpResult PalaceService::setBookingStatus(const string& bookingId, BookingStatus status) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Booking đã hủy.");
    if (status == BookingStatus::ChoCoc || status == BookingStatus::DaCoc)
        return OpResult::fail("Trạng thái Đã đặt cọc do hệ thống tự cập nhật khi thu tiền.");
    if (status == BookingStatus::HoanTat && b->status() != BookingStatus::DaCoc)
        return OpResult::fail("Chỉ hoàn tất booking đã đặt cọc.");
    b->setStatus(status);
    return persist("Đã cập nhật trạng thái booking.", bookingId);
}

OpResult PalaceService::cancelBooking(const string& bookingId, const string& reason) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Booking đã hủy.");
    b->setStatus(BookingStatus::DaHuy);
    b->setCancelReason(trim(reason));
    for (auto& ev : mut().events) {
        if (ev.bookingId() == bookingId) ev.setStatus(EventStatus::Huy);
    }
    return persist("Đã hủy booking. Sảnh được giải phóng lịch.", bookingId);
}

OpResult PalaceService::approveDiscount(const string& bookingId) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (!b->discountNeedsApproval()) return OpResult::fail("Booking không cần duyệt chiết khấu.");
    b->setDiscountApproved(true);
    return persist("Quản lý đã duyệt chiết khấu cho booking " + bookingId + ".", bookingId);
}

OpResult PalaceService::updateBookingDiscount(const string& bookingId, int discountPercent,
                                              int64_t discountAmount) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() != BookingStatus::ChoCoc)
        return OpResult::fail("Chỉ sửa giảm giá khi booking đang chờ cọc.");
    if (discountPercent < 0 || discountAmount < 0) return OpResult::fail("Chiết khấu không âm.");
    if (discountPercent > 100) return OpResult::fail("Chiết khấu % tối đa 100.");
    if (discountAmount > b->estimatedTotal()) return OpResult::fail("Giảm giá vượt tổng chi phí.");
    const User* actor = currentUser();
    bool needsApproval = (discountPercent > kDiscountApprovalPercent ||
                           discountAmount > discountLimitFor(b->estimatedTotal())) &&
                          !(actor && actor->role() == Role::Manager);
    b->setDiscount(discountPercent, discountAmount, needsApproval);
    if (needsApproval)
        return persist("Đã cập nhật giảm giá. Vượt " + to_string(kDiscountApprovalPercent) +
                           "% cần Quản lý duyệt (bấm vào ô Giảm giá).",
                       bookingId);
    return persist("Đã cập nhật giảm giá.", bookingId);
}

OpResult PalaceService::deleteBooking(const string& bookingId) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaCoc || b->status() == BookingStatus::HoanTat)
        return OpResult::fail("Booking đã cọc/hoàn tất, hãy dùng Hủy booking thay vì xóa.");
    for (const auto& p : data().payments) {
        if (p.bookingId() == bookingId && p.status() == PaymentStatus::DaThanhToan)
            return OpResult::fail("Booking đã có khoản thu, hãy dùng Hủy booking thay vì xóa.");
    }
    // Xoa keo theo khoan tu tao chua thu + su kien cua rieng booking nay.
    {
        auto& pays = mut().payments;
        pays.erase(remove_if(pays.begin(), pays.end(),
                             [&](const Payment& p) {
                                 return p.bookingId() == bookingId &&
                                        p.status() == PaymentStatus::ChuaThanhToan;
                             }),
                   pays.end());
    }
    {
        auto& evs = mut().events;
        evs.erase(remove_if(evs.begin(), evs.end(),
                            [&](const Event& e) { return e.bookingId() == bookingId; }),
                  evs.end());
    }
    {
        auto& items = mut().bookings;
        items.erase(remove_if(items.begin(), items.end(),
                              [&](const Booking& x) { return x.id() == bookingId; }),
                    items.end());
    }
    return persist("Đã xóa booking.", bookingId);
}

vector<Booking> PalaceService::bookingsForCustomer(const string& customerId) const {
    vector<Booking> out;
    for (const auto& b : data().bookings)
        if (b.customerId() == customerId) out.push_back(b);
    return out;
}

int PalaceService::sweepExpiredHolds(DateTime now) {
    int n = 0;
    for (auto& b : mut().bookings) {
        if (b.status() != BookingStatus::ChoCoc) continue;
        if (!(b.holdUntil() < now)) continue;
        bool deposited = false;
        for (const auto& p : data().payments) {
            if (p.bookingId() == b.id() && p.status() == PaymentStatus::DaThanhToan &&
                (p.type() == PaymentType::DatCoc1 || p.type() == PaymentType::DatCoc2)) {
                deposited = true;
                break;
            }
        }
        if (!deposited) {
            b.setStatus(BookingStatus::DaHuy);
            b.setCancelReason("Quá hạn giữ chỗ " + to_string(kHoldHours) + "h chưa đặt cọc.");
            for (auto& ev : mut().events) {
                if (ev.bookingId() == b.id()) ev.setStatus(EventStatus::Huy);
            }
            ++n;
        }
    }
    return n;
}

}  // namespace tdp
