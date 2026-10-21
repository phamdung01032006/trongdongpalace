#include "PalaceService.h"

#include "Utils.h"

#include <algorithm>

using namespace std;

// ==================== YEU CAU SU KIEN ====================

const EventRequest* PalaceService::findRequest(const string& id) const {
    return findById(get_db().get_requests(), id);
}

int PalaceService::maxHallCapacity() const {
    int maximum = 0;
    for (const Hall& hall : get_db().get_halls()) maximum = max(maximum, hall.get_capacity());
    return maximum > 0 ? maximum : 800;
}

void PalaceService::buildLines(int guestCount, const vector<ServiceLineInput>& input,
                               vector<RequestLine>& output) const {
    output.clear();

    for (const ServiceLineInput& item : input) {
        const Service* service = findService(item.serviceId);
        if (service == nullptr) throw BusinessException("Dịch vụ không tồn tại: " + item.serviceId);
        if (!service->is_active()) throw BusinessException("Dịch vụ đã ngừng cung cấp: " + service->get_name());

        // Goi ham ao quantityFor(): FoodService va PackageService tu tinh khac nhau.
        int quantity = service->quantityFor(guestCount, item.quantity);
        output.push_back(RequestLine(service->get_id(), service->get_name(), service->get_unit(),
                                     quantity, service->get_unitPrice()));
    }
}

namespace {

// Kiem tra cac dieu kien chung cua mot yeu cau.
bool validRequestWindow(DateTime start, DateTime end, int guestCount, int maxCapacity,
                        string& error) {
    if (!start.is_valid() || !end.is_valid()) {
        error = "Ngày giờ không hợp lệ.";
        return false;
    }
    if (!(start < end)) {
        error = "Thời gian bắt đầu phải sớm hơn thời gian kết thúc.";
        return false;
    }
    if (start < DateTime::now().plusHours(-1)) {
        error = "Ngày diễn ra phải ở tương lai.";
        return false;
    }
    if (guestCount < 10 || guestCount > maxCapacity) {
        error = "Số lượng khách phải từ 10 đến " + to_string(maxCapacity) + ".";
        return false;
    }
    return true;
}

}  // namespace

OpResult PalaceService::addRequest(const string& customerId, EventType type, const string& title,
                                   const string& customerName, const string& customerPhone,
                                   const string& hallId, DateTime start, DateTime end,
                                   int guestCount, const string& notes,
                                   const vector<ServiceLineInput>& services) {
    require(Action::ManageRequests);

    const User* actor = currentUser();
    string finalCustomerId = customerId;
    string finalName = trim(customerName);
    string finalPhone = trim(customerPhone);

    // Khach hang chi duoc tao yeu cau cho chinh minh.
    if (actor != nullptr && actor->get_role() == Role::Customer) {
        if (actor->get_customerId().empty()) throw BusinessException("Tài khoản chưa gắn hồ sơ khách hàng.");
        finalCustomerId = actor->get_customerId();

        const Customer* self = findCustomer(finalCustomerId);
        if (self != nullptr) {
            if (finalName.empty()) finalName = self->get_fullName();
            if (finalPhone.empty()) finalPhone = self->get_phone();
        }
    }

    if (findCustomer(finalCustomerId) == nullptr) throw BusinessException("Khách hàng không tồn tại.");
    if (trim(title).empty()) throw BusinessException("Tên sự kiện là bắt buộc.");
    if (finalName.empty()) throw BusinessException("Họ tên khách hàng là bắt buộc.");
    if (!looksLikePhone(finalPhone)) throw BusinessException("Số điện thoại không hợp lệ.");
    if (!hallId.empty() && findHall(hallId) == nullptr)
        throw BusinessException("Sảnh mong muốn không tồn tại.");

    string error;
    if (!validRequestWindow(start, end, guestCount, maxHallCapacity(), error))
        throw BusinessException(error);

    vector<RequestLine> lines;
    buildLines(guestCount, services, lines);

    // Khach sua ten/SDT tren form -> dong bo nguoc ve ho so cua chinh ho.
    if (actor != nullptr && actor->get_role() == Role::Customer) {
        for (Customer& self : get_db().get_customers()) {
            if (self.get_id() != finalCustomerId) continue;
            if (finalName != self.get_fullName() || finalPhone != self.get_phone()) {
                if (!uniquePhone(finalPhone, finalCustomerId))
                    throw BusinessException("Số điện thoại đã được dùng cho hồ sơ khác.");
                self.set_fullName(finalName);
                self.set_phone(finalPhone);
            }
            break;
        }
    }

    string id = nextId("REQ-", requestIds());
    EventRequest request(id, finalCustomerId, type, trim(title), finalName, finalPhone, hallId,
                         start, end, guestCount, trim(notes), RequestStatus::Cho);
    request.get_lines() = lines;

    int64_t estimate = request.get_servicesTotal();
    if (const Hall* hall = findHall(hallId)) estimate += hall->get_rentalPrice();

    get_db().get_requests().push_back(request);
    return persist("Đã gửi yêu cầu " + id + ". Chi phí dự kiến: " + formatMoney(estimate), id);
}

OpResult PalaceService::updateRequest(const string& id, EventType type, const string& title,
                                      const string& customerName, const string& customerPhone,
                                      const string& hallId, DateTime start, DateTime end,
                                      int guestCount, const string& notes,
                                      const vector<ServiceLineInput>& services) {
    require(Action::ManageRequests);

    EventRequest* request = nullptr;
    for (EventRequest& item : get_db().get_requests()) {
        if (item.get_id() == id) {
            request = &item;
            break;
        }
    }
    if (request == nullptr) throw BusinessException("Không tìm thấy yêu cầu.");

    const User* actor = currentUser();
    if (actor != nullptr && actor->get_role() == Role::Customer &&
        request->get_customerId() != actor->get_customerId()) {
        throw BusinessException("Bạn chỉ sửa yêu cầu của mình.");
    }
    if (request->get_status() == RequestStatus::DaChot || request->get_status() == RequestStatus::BiHuy)
        throw BusinessException("Yêu cầu đã chốt/hủy, không cập nhật.");

    if (trim(title).empty()) throw BusinessException("Tên sự kiện là bắt buộc.");
    if (!looksLikePhone(trim(customerPhone))) throw BusinessException("Số điện thoại không hợp lệ.");
    if (!hallId.empty() && findHall(hallId) == nullptr)
        throw BusinessException("Sảnh mong muốn không tồn tại.");

    string error;
    if (!validRequestWindow(start, end, guestCount, maxHallCapacity(), error))
        throw BusinessException(error);

    vector<RequestLine> lines;
    buildLines(guestCount, services, lines);

    request->set_type(type);
    request->set_title(trim(title));
    request->set_customerName(trim(customerName));
    request->set_customerPhone(trim(customerPhone));
    request->set_hallId(hallId);
    request->set_start(start);
    request->set_end(end);
    request->set_guestCount(guestCount);
    request->set_notes(trim(notes));
    request->get_lines() = lines;

    return persist("Đã cập nhật yêu cầu " + id + ". Chi phí dự kiến: " +
                       formatMoney(estimateRequest(*request)),
                   id);
}

OpResult PalaceService::updateRequestStatus(const string& id, RequestStatus status) {
    require(Action::ManageRequests);

    EventRequest* request = nullptr;
    for (EventRequest& item : get_db().get_requests()) {
        if (item.get_id() == id) {
            request = &item;
            break;
        }
    }
    if (request == nullptr) throw BusinessException("Không tìm thấy yêu cầu.");

    const User* actor = currentUser();

    // Khach hang chi duoc HUY yeu cau cua minh.
    if (actor != nullptr && actor->get_role() == Role::Customer) {
        if (request->get_customerId() != actor->get_customerId())
            throw BusinessException("Bạn chỉ xử lý yêu cầu của mình.");
        if (status != RequestStatus::BiHuy)
            throw BusinessException("Khách hàng chỉ được hủy yêu cầu.");
        if (request->get_status() == RequestStatus::DaChot || request->get_status() == RequestStatus::BiHuy)
            throw BusinessException("Yêu cầu đã kết thúc, không thể hủy.");

        request->set_status(RequestStatus::BiHuy);
        return persist("Đã hủy yêu cầu " + id, id);
    }

    if (request->get_status() == RequestStatus::DaChot || request->get_status() == RequestStatus::BiHuy)
        throw BusinessException("Yêu cầu đã chốt/hủy, không đổi trạng thái.");

    if (status == RequestStatus::Cho || status == RequestStatus::DangTuVan) {
        request->set_status(status);
        return persist("Đã cập nhật trạng thái yêu cầu.", id);
    }

    if (status == RequestStatus::BiHuy) {
        request->set_status(RequestStatus::BiHuy);
        return persist("Đã hủy yêu cầu.", id);
    }

    if (status == RequestStatus::HoanThanh || status == RequestStatus::DaChot) {
        // Yeu cau Hoan thanh -> tu dong tao Booking.
        const Hall* hall = findHall(request->get_hallId());
        if (hall == nullptr) {
            vector<Hall> available = findAvailableHalls(request->get_start(), request->get_end(),
                                                        request->get_guestCount());
            if (available.empty())
                throw BusinessException("Không còn sảnh trống, cần tư vấn lại với khách.");
            hall = findHall(available.front().get_id());
        }

        string bookingId;
        applyBookingFromRequest(
            request->get_id(), hall->get_id(), request->get_start(), request->get_end(),
            request->get_guestCount(), bookingId);

        // Neu phai doi sang sanh khac, cap nhat lai sanh tren yeu cau.
        request->set_hallId(hall->get_id());
        return persist("Yêu cầu hoàn thành, đã chuyển sang Booking " + bookingId + ".", bookingId);
    }

    throw BusinessException("Trạng thái không hợp lệ.");
}

OpResult PalaceService::cancelOwnRequest(const string& id) {
    return updateRequestStatus(id, RequestStatus::BiHuy);
}

OpResult PalaceService::deleteRequest(const string& id) {
    require(Action::ManageRequests);

    const EventRequest* request = findRequest(id);
    if (request == nullptr) throw BusinessException("Không tìm thấy yêu cầu.");

    const User* actor = currentUser();
    if (actor != nullptr && actor->get_role() == Role::Customer &&
        request->get_customerId() != actor->get_customerId()) {
        throw BusinessException("Bạn chỉ xóa yêu cầu của mình.");
    }
    if (request->get_status() == RequestStatus::DaChot)
        throw BusinessException("Yêu cầu đã chốt booking, không thể xóa.");
    for (const Booking& booking : get_db().get_bookings())
        if (booking.get_requestId() == id) throw BusinessException("Yêu cầu đã có booking, không thể xóa.");

    vector<EventRequest>& requests = get_db().get_requests();
    requests.erase(remove_if(requests.begin(), requests.end(),
                             [&](const EventRequest& item) { return item.get_id() == id; }),
                   requests.end());
    return persist("Đã xóa yêu cầu.", id);
}

int64_t PalaceService::estimateRequest(const EventRequest& request) const {
    int64_t total = request.get_servicesTotal();
    if (const Hall* hall = findHall(request.get_hallId())) total += hall->get_rentalPrice();
    return total;
}

vector<EventRequest> PalaceService::requestHistory(size_t limit) const {
    vector<EventRequest> result;
    for (const EventRequest& request : get_db().get_requests()) {
        if (request.get_status() == RequestStatus::BiHuy ||
            request.get_status() == RequestStatus::DaChot ||
            request.get_status() == RequestStatus::HoanThanh) {
            result.push_back(request);
        }
    }

    sort(result.begin(), result.end(), [](const EventRequest& a, const EventRequest& b) {
        return b.get_createdAt() < a.get_createdAt();
    });

    if (result.size() > limit) result.resize(limit);
    return result;
}

size_t PalaceService::countNewRequests() const {
    size_t count = 0;
    for (const EventRequest& request : get_db().get_requests())
        if (request.get_status() == RequestStatus::Cho) ++count;
    return count;
}

vector<EventRequest> PalaceService::requestsForCustomer(const string& customerId) const {
    vector<EventRequest> result;
    for (const EventRequest& request : get_db().get_requests())
        if (request.get_customerId() == customerId) result.push_back(request);

    sort(result.begin(), result.end(), [](const EventRequest& a, const EventRequest& b) {
        return b.get_createdAt() < a.get_createdAt();
    });
    return result;
}
