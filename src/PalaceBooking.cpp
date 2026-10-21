#include "PalaceService.h"

#include "Utils.h"

#include <algorithm>
#include <cstddef>
#include <sstream>

using namespace std;

namespace {

int64_t discountLimitFor(int64_t total) {
    return total * PalaceService::kDiscountApprovalPercent / 100;
}

}  // namespace

// ==================== SANH & DICH VU ====================

const Hall* PalaceService::findHall(const string& id) const {
    return findById(get_db().get_halls(), id);
}

const Service* PalaceService::findService(const string& idOrCode) const {
    for (const Service* service : get_db().get_services()) {
        if (service->get_id() == idOrCode) return service;
    }
    for (const Service* service : get_db().get_services()) {
        if (toLower(service->get_code()) == toLower(idOrCode)) return service;
    }
    return nullptr;
}

Service* PalaceService::findServiceMutable(const string& idOrCode) {
    for (Service* service : get_db().get_services()) {
        if (service->get_id() == idOrCode) return service;
    }
    for (Service* service : get_db().get_services()) {
        if (toLower(service->get_code()) == toLower(idOrCode)) return service;
    }
    return nullptr;
}

OpResult PalaceService::addHall(const string& name, int capacity, int64_t rentalPrice) {
    require(Action::ManageVenues);
    if (trim(name).empty()) throw BusinessException("Tên sảnh là bắt buộc.");
    if (capacity <= 0) throw BusinessException("Sức chứa phải lớn hơn 0.");
    if (rentalPrice < 0) throw BusinessException("Giá thuê không được âm.");

    vector<string> hallIds;
    for (const Hall& hall : get_db().get_halls()) hallIds.push_back(hall.get_id());
    string id = nextId("HAL-", hallIds);

    get_db().get_halls().push_back(Hall(id, trim(name), capacity, rentalPrice, HallStatus::KhaDung));
    return persist("Đã thêm sảnh " + id, id);
}

OpResult PalaceService::updateHall(const string& id, const string& name, int capacity,
                                   int64_t rentalPrice, HallStatus status) {
    require(Action::ManageVenues);

    for (Hall& hall : get_db().get_halls()) {
        if (hall.get_id() != id) continue;
        if (trim(name).empty()) throw BusinessException("Tên sảnh là bắt buộc.");
        if (capacity <= 0) throw BusinessException("Sức chứa phải lớn hơn 0.");
        if (rentalPrice < 0) throw BusinessException("Giá thuê không được âm.");

        hall.set_name(trim(name));
        hall.set_capacity(capacity);
        hall.set_rentalPrice(rentalPrice);
        hall.set_status(status);
        return persist("Đã cập nhật sảnh.", id);
    }
    throw BusinessException("Không tìm thấy sảnh.");
}

OpResult PalaceService::deleteHall(const string& id) {
    require(Action::ManageVenues);
    if (findHall(id) == nullptr) throw BusinessException("Không tìm thấy sảnh.");

    for (const Booking& booking : get_db().get_bookings())
        if (booking.get_hallId() == id) throw BusinessException("Sảnh đã có booking, không thể xóa.");
    for (const EventRequest& request : get_db().get_requests())
        if (request.get_hallId() == id)
            throw BusinessException("Sảnh đang được yêu cầu chọn, không thể xóa.");

    vector<Hall>& halls = get_db().get_halls();
    halls.erase(remove_if(halls.begin(), halls.end(),
                          [&](const Hall& hall) { return hall.get_id() == id; }),
                halls.end());
    return persist("Đã xóa sảnh.", id);
}

OpResult PalaceService::addService(const string& code, const string& name, const string& unit,
                                   int64_t price, const string& category, bool isFood) {
    require(Action::ManageVenues);
    if (trim(name).empty()) throw BusinessException("Tên dịch vụ là bắt buộc.");
    if (price < 0) throw BusinessException("Đơn giá không được âm.");
    if (!trim(code).empty() && findService(trim(code)) != nullptr)
        throw BusinessException("Mã dịch vụ đã tồn tại.");

    vector<string> ids;
    for (const Service* service : get_db().get_services()) ids.push_back(service->get_id());
    string id = nextId("SVC-", ids);
    string finalCode = trim(code).empty() ? id : trim(code);
    string finalUnit = trim(unit).empty() ? "VNĐ/gói" : trim(unit);
    string finalCategory = trim(category).empty() ? "Khác" : trim(category);

    if (isFood) {
        get_db().get_services().push_back(new FoodService(id, finalCode, trim(name), finalCategory,
                                                 finalUnit, price, true));
    } else {
        get_db().get_services().push_back(new PackageService(id, finalCode, trim(name), finalCategory,
                                                    finalUnit, price, true));
    }
    return persist("Đã thêm dịch vụ " + finalCode, id);
}

OpResult PalaceService::updateService(const string& id, const string& code, const string& name,
                                      const string& unit, int64_t price,
                                      const string& category, bool active) {
    require(Action::ManageVenues);

    Service* target = findServiceMutable(id);
    if (target == nullptr) throw BusinessException("Không tìm thấy dịch vụ.");
    if (trim(name).empty()) throw BusinessException("Tên dịch vụ là bắt buộc.");
    if (price < 0) throw BusinessException("Đơn giá không được âm.");

    string newCode = trim(code).empty() ? target->get_id() : trim(code);
    const Service* duplicate = findService(newCode);
    if (duplicate != nullptr && duplicate->get_id() != target->get_id())
        throw BusinessException("Mã dịch vụ đã tồn tại.");

    target->set_code(newCode);
    target->set_name(trim(name));
    target->set_unit(trim(unit));
    target->set_unitPrice(price);
    target->set_category(trim(category));
    target->set_active(active);
    return persist("Đã cập nhật dịch vụ (các yêu cầu cũ vẫn giữ đơn giá cũ).", id);
}

OpResult PalaceService::deleteService(const string& id) {
    require(Action::ManageVenues);

    Service* target = findServiceMutable(id);
    if (target == nullptr) throw BusinessException("Không tìm thấy dịch vụ.");

    for (const EventRequest& request : get_db().get_requests()) {
        for (const RequestLine& line : request.get_lines()) {
            if (line.get_serviceId() == target->get_id())
                throw BusinessException("Dịch vụ đang dùng trong yêu cầu " + request.get_id() +
                                      ", không thể xóa.");
        }
    }

    string realId = target->get_id();
    for (size_t i = 0; i < get_db().get_services().size(); ++i) {
        if (get_db().get_services()[i]->get_id() == realId) {
            delete get_db().get_services()[i];
            get_db().get_services().erase(get_db().get_services().begin() + static_cast<ptrdiff_t>(i));
            break;
        }
    }
    return persist("Đã xóa dịch vụ.", realId);
}

OpResult PalaceService::seedCatalog() {
    require(Action::ManageVenues);

    store_.seedCatalog();
    return persist("Đã nạp danh mục mẫu (chỉ thêm mục còn thiếu).", "");
}

bool PalaceService::hallAvailable(const string& hallId, DateTime start, DateTime end,
                                  const string& ignoreBookingId, string& reason) const {
    const Hall* hall = findHall(hallId);
    if (hall == nullptr) {
        reason = "Sảnh không tồn tại.";
        return false;
    }
    if (!hall->is_usable()) {
        reason = "Sảnh '" + hall->get_name() + "' hiện ở trạng thái " + toString(hall->get_status()) + ".";
        return false;
    }
    if (!(start < end)) {
        reason = "Thời gian bắt đầu phải sớm hơn thời gian kết thúc.";
        return false;
    }

    for (const Booking& booking : get_db().get_bookings()) {
        if (!booking.is_occupying_hall()) continue;
        if (booking.get_id() == ignoreBookingId) continue;
        if (booking.get_hallId() != hallId) continue;
        if (timeOverlaps(start, end, booking.get_start(), booking.get_end())) {
            reason = "Sảnh bị trùng lịch với booking " + booking.get_id() + " (" +
                     booking.get_start().toString() + " - " + booking.get_end().toString() + ").";
            return false;
        }
    }
    return true;
}

vector<Hall> PalaceService::findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                               const string& ignoreBookingId) const {
    vector<Hall> result;
    string reason;
    for (const Hall& hall : get_db().get_halls()) {
        if (guestCount > hall.get_capacity()) continue;
        if (hallAvailable(hall.get_id(), start, end, ignoreBookingId, reason)) result.push_back(hall);
    }
    return result;
}

vector<Hall> PalaceService::filterHalls(DateTime start, DateTime end, int guestCount,
                                        int64_t maxPrice, const string& ignoreBookingId) const {
    vector<Hall> result;
    for (const Hall& hall : findAvailableHalls(start, end, guestCount, ignoreBookingId)) {
        if (maxPrice > 0 && hall.get_rentalPrice() > maxPrice) continue;
        result.push_back(hall);
    }
    return result;
}

// ==================== BOOKING ====================

const Booking* PalaceService::findBooking(const string& id) const {
    return findById(get_db().get_bookings(), id);
}

OpResult PalaceService::applyBookingFromRequest(const string& requestId, const string& hallId,
                                                DateTime start, DateTime end, int guestCount,
                                                int discountPercent, int64_t discountAmount,
                                                string& bookingId) {
    EventRequest* request = nullptr;
    for (EventRequest& item : get_db().get_requests()) {
        if (item.get_id() == requestId) {
            request = &item;
            break;
        }
    }
    if (request == nullptr) throw BusinessException("Yêu cầu sự kiện không tồn tại.");
    if (request->get_status() == RequestStatus::BiHuy || request->get_status() == RequestStatus::DaChot)
        throw BusinessException("Yêu cầu đã hủy/chốt, không tạo booking.");

    const Hall* hall = findHall(hallId);
    if (hall == nullptr) throw BusinessException("Sảnh chốt không tồn tại.");
    if (guestCount <= 0) throw BusinessException("Số khách phải lớn hơn 0.");
    if (guestCount > hall->get_capacity()) throw BusinessException("Số khách vượt sức chứa sảnh.");

    string reason;
    if (!hallAvailable(hallId, start, end, "", reason)) throw BusinessException(reason);

    int64_t total = hall->get_rentalPrice() + request->get_servicesTotal();

    bool needsApproval = false;
    const User* actor = currentUser();
    bool isManager = actor != nullptr && actor->get_role() == Role::Manager;

    if (discountPercent > 0 || discountAmount > 0) {
        if (discountPercent < 0 || discountAmount < 0)
            throw BusinessException("Chiết khấu không được âm.");
        if (discountPercent > 100) throw BusinessException("Chiết khấu % tối đa 100.");
        if (discountAmount > total) throw BusinessException("Giảm giá vượt tổng chi phí.");

        if ((discountPercent > kDiscountApprovalPercent ||
             discountAmount > discountLimitFor(total)) &&
            !isManager) {
            needsApproval = true;
        }
    }

    vector<string> ids;
    for (const Booking& booking : get_db().get_bookings()) ids.push_back(booking.get_id());
    string id = nextId("BKG-", ids);

    Booking booking(id, requestId, request->get_customerId(), hallId, start, end, guestCount, total,
                    BookingStatus::ChoCoc);
    DateTime now = DateTime::now();
    booking.set_createdAt(now);
    booking.set_holdUntil(now.plusHours(kHoldHours));
    booking.set_salesUsername(actor != nullptr ? actor->get_username() : "");
    booking.set_discount(discountPercent, discountAmount, needsApproval);
    get_db().get_bookings().push_back(booking);

    request->set_status(RequestStatus::DaChot);

    // Tao san khoan dat coc (amount = 0, cho nhan vien dien).
    string paymentId = nextId("PAY-", paymentIds());
    get_db().get_payments().push_back(Payment(paymentId, id, PaymentType::DatCoc1, 0, now,
                                     PaymentMethod::ChuyenKhoan,
                                     PaymentStatus::ChuaThanhToan,
                                     "Tiền đặt cọc giữ chỗ",
                                     actor != nullptr ? actor->get_username() : ""));

    bookingId = id;
    if (needsApproval) {
        return persist("Đã tạo booking " + id + " (giữ chỗ " + to_string(kHoldHours) +
                           " giờ). Chiết khấu vượt " + to_string(kDiscountApprovalPercent) +
                           "% cần Quản lý duyệt.",
                       id);
    }
    return persist("Đã tạo booking " + id + " từ yêu cầu " + requestId + " (giữ chỗ " +
                       to_string(kHoldHours) + " giờ chờ đặt cọc).",
                   id);
}

OpResult PalaceService::createBooking(const string& requestId, const string& hallId,
                                      DateTime start, DateTime end, int guestCount,
                                      int discountPercent, int64_t discountAmount) {
    require(Action::ManageBookings);

    string bookingId;
    return applyBookingFromRequest(requestId, hallId, start, end, guestCount, discountPercent,
                                   discountAmount, bookingId);
}

OpResult PalaceService::cancelBooking(const string& bookingId, const string& reason) {
    require(Action::ManageBookings);

    for (Booking& booking : get_db().get_bookings()) {
        if (booking.get_id() != bookingId) continue;
        if (booking.get_status() == BookingStatus::DaHuy)
            throw BusinessException("Booking đã hủy.");

        booking.set_status(BookingStatus::DaHuy);
        booking.set_cancelReason(trim(reason));

        for (Event& event : get_db().get_events()) {
            if (event.get_bookingId() == bookingId) event.set_status(EventStatus::DaHuy);
        }
        return persist("Đã hủy booking. Sảnh được giải phóng lịch.", bookingId);
    }
    throw BusinessException("Không tìm thấy booking.");
}

OpResult PalaceService::deleteBooking(const string& bookingId) {
    require(Action::ManageBookings);

    const Booking* target = findBooking(bookingId);
    if (target == nullptr) throw BusinessException("Không tìm thấy booking.");
    if (target->get_status() == BookingStatus::DaCoc || target->get_status() == BookingStatus::HoanTat)
        throw BusinessException("Booking đã cọc/hoàn tất, hãy dùng Hủy booking thay vì xóa.");

    for (const Payment& payment : get_db().get_payments()) {
        if (payment.get_bookingId() == bookingId && payment.get_status() == PaymentStatus::DaThanhToan)
            throw BusinessException("Booking đã có khoản thu, hãy dùng Hủy booking thay vì xóa.");
    }

    // Xoa kem khoan chua thu va su kien cua booking nay.
    // Tra yeu cau ve "Hoan thanh" de co the tao booking lai neu can.
    for (EventRequest& request : get_db().get_requests()) {
        if (request.get_id() == target->get_requestId() && request.get_status() == RequestStatus::DaChot) {
            request.set_status(RequestStatus::HoanThanh);
            break;
        }
    }

    vector<Payment>& payments = get_db().get_payments();
    payments.erase(remove_if(payments.begin(), payments.end(),
                             [&](const Payment& payment) {
                                 return payment.get_bookingId() == bookingId &&
                                        payment.get_status() == PaymentStatus::ChuaThanhToan;
                             }),
                   payments.end());

    vector<Event>& events = get_db().get_events();
    events.erase(remove_if(events.begin(), events.end(),
                           [&](const Event& event) { return event.get_bookingId() == bookingId; }),
                 events.end());

    vector<Booking>& bookings = get_db().get_bookings();
    bookings.erase(remove_if(bookings.begin(), bookings.end(),
                             [&](const Booking& booking) { return booking.get_id() == bookingId; }),
                   bookings.end());

    return persist("Đã xóa booking.", bookingId);
}

OpResult PalaceService::updateBookingDiscount(const string& bookingId, int discountPercent,
                                              int64_t discountAmount) {
    require(Action::ManageBookings);

    for (Booking& booking : get_db().get_bookings()) {
        if (booking.get_id() != bookingId) continue;
        if (booking.get_status() != BookingStatus::ChoCoc)
            throw BusinessException("Chỉ sửa giảm giá khi booking đang chờ cọc.");
        if (discountPercent < 0 || discountAmount < 0)
            throw BusinessException("Chiết khấu không được âm.");
        if (discountPercent > 100) throw BusinessException("Chiết khấu % tối đa 100.");
        if (discountAmount > booking.get_estimatedTotal())
            throw BusinessException("Giảm giá vượt tổng chi phí.");

        const User* actor = currentUser();
        bool needsApproval =
            (discountPercent > kDiscountApprovalPercent ||
             discountAmount > discountLimitFor(booking.get_estimatedTotal())) &&
            !(actor != nullptr && actor->get_role() == Role::Manager);

        booking.set_discount(discountPercent, discountAmount, needsApproval);
        if (needsApproval) {
            return persist("Đã cập nhật giảm giá. Vượt " + to_string(kDiscountApprovalPercent) +
                               "% cần Quản lý duyệt.",
                           bookingId);
        }
        return persist("Đã cập nhật giảm giá.", bookingId);
    }
    throw BusinessException("Không tìm thấy booking.");
}

OpResult PalaceService::approveDiscount(const string& bookingId) {
    require(Action::ManageVenues);

    for (Booking& booking : get_db().get_bookings()) {
        if (booking.get_id() != bookingId) continue;
        if (!booking.get_discountNeedsApproval())
            throw BusinessException("Booking không cần duyệt chiết khấu.");
        booking.set_discountApproved(true);
        return persist("Quản lý đã duyệt chiết khấu cho booking " + bookingId + ".", bookingId);
    }
    throw BusinessException("Không tìm thấy booking.");
}

vector<Booking> PalaceService::bookingsForCustomer(const string& customerId) const {
    vector<Booking> result;
    for (const Booking& booking : get_db().get_bookings())
        if (booking.get_customerId() == customerId) result.push_back(booking);
    return result;
}

// ==================== THANH TOAN ====================

int64_t PalaceService::paidAmount(const string& bookingId) const {
    int64_t total = 0;
    for (const Payment& payment : get_db().get_payments()) {
        if (payment.get_bookingId() == bookingId &&
            payment.get_status() == PaymentStatus::DaThanhToan && payment.get_amount() > 0) {
            total += payment.get_amount();
        }
    }
    return total;
}

vector<Payment> PalaceService::paymentsOf(const string& bookingId) const {
    vector<Payment> result;
    for (const Payment& payment : get_db().get_payments())
        if (payment.get_bookingId() == bookingId) result.push_back(payment);
    return result;
}

OpResult PalaceService::recordPayment(const string& bookingId, PaymentType type, int64_t amount,
                                      PaymentMethod method, DateTime paidAt,
                                      const string& note) {
    require(Action::ManagePayments);

    const Booking* booking = findBooking(bookingId);
    if (booking == nullptr) throw BusinessException("Booking không tồn tại.");
    if (booking->get_status() == BookingStatus::DaHuy)
        throw BusinessException("Không thu tiền cho booking đã hủy.");
    if (amount <= 0) throw BusinessException("Số tiền thanh toán phải lớn hơn 0.");
    if (trim(note).empty()) throw BusinessException("Ghi chú thanh toán là bắt buộc.");
    if (!paidAt.is_valid()) throw BusinessException("Ngày thanh toán không hợp lệ.");
    if (DateTime::now() < paidAt)
        throw BusinessException("Ngày thanh toán không được ở tương lai.");

    int64_t remaining = booking->get_finalTotal() - paidAmount(bookingId);
    if (remaining < 0) remaining = 0;
    if (remaining == 0) throw BusinessException("Booking đã thu đủ tiền.");
    if (amount > remaining)
        throw BusinessException("Số tiền vượt quá số còn lại (" + formatMoney(remaining) + ").");
    if (type == PaymentType::TatToan && amount < remaining)
        throw BusinessException("Tất toán phải thu đủ số còn lại (" + formatMoney(remaining) + ").");

    const User* actor = currentUser();
    string paymentId;
    bool filled = false;

    // Neu da co khoan cung loai dang cho thu thi dien vao khoan do.
    for (Payment& payment : get_db().get_payments()) {
        if (payment.get_bookingId() == bookingId && payment.get_type() == type &&
            payment.get_status() == PaymentStatus::ChuaThanhToan) {
            payment.set_amount(amount);
            payment.set_method(method);
            payment.set_paidAt(paidAt);
            payment.set_note(trim(note));
            payment.set_status(PaymentStatus::DaThanhToan);
            paymentId = payment.get_id();
            filled = true;
            break;
        }
    }

    if (!filled) {
        paymentId = nextId("PAY-", paymentIds());
        get_db().get_payments().push_back(Payment(paymentId, bookingId, type, amount, paidAt, method,
                                         PaymentStatus::DaThanhToan, trim(note),
                                         actor != nullptr ? actor->get_username() : ""));
    }

    applyPaymentCompleted(bookingId, type);
    return persist("Đã ghi nhận " + toString(type) + " " + formatMoney(amount) +
                       " cho booking " + bookingId + ".",
                   paymentId);
}

OpResult PalaceService::updatePayment(const string& id, PaymentType type, int64_t amount,
                                      PaymentMethod method, DateTime paidAt,
                                      const string& note) {
    require(Action::ManagePayments);

    for (Payment& payment : get_db().get_payments()) {
        if (payment.get_id() != id) continue;
        if (amount <= 0) throw BusinessException("Số tiền thanh toán phải lớn hơn 0.");
        if (trim(note).empty()) throw BusinessException("Ghi chú thanh toán là bắt buộc.");
        if (!paidAt.is_valid()) throw BusinessException("Ngày thanh toán không hợp lệ.");
        if (DateTime::now() < paidAt)
            throw BusinessException("Ngày thanh toán không được ở tương lai.");

        const Booking* booking = findBooking(payment.get_bookingId());
        if (booking == nullptr)
            throw BusinessException("Booking của khoản thanh toán không tồn tại.");
        if (booking->get_status() == BookingStatus::DaHuy)
            throw BusinessException("Không cập nhật thanh toán cho booking đã hủy.");
        int64_t paidBefore = paidAmount(payment.get_bookingId());
        if (payment.get_status() == PaymentStatus::DaThanhToan) paidBefore -= payment.get_amount();
        if (paidBefore < 0) paidBefore = 0;
        int64_t remaining = booking->get_finalTotal() - paidBefore;
        if (remaining < 0) remaining = 0;
        if (remaining == 0) throw BusinessException("Booking đã thu đủ tiền.");
        if (amount > remaining)
            throw BusinessException("Số tiền vượt quá số còn lại (" + formatMoney(remaining) + ").");
        if (type == PaymentType::TatToan && amount < remaining)
            throw BusinessException("Tất toán phải thu đủ số còn lại (" + formatMoney(remaining) + ").");

        payment.set_type(type);
        payment.set_amount(amount);
        payment.set_method(method);
        payment.set_paidAt(paidAt);
        payment.set_note(trim(note));
        payment.set_status(PaymentStatus::DaThanhToan);
        applyPaymentCompleted(payment.get_bookingId(), type);
        return persist("Đã cập nhật khoản thanh toán " + id + ".", id);
    }
    throw BusinessException("Không tìm thấy khoản thanh toán.");
}

OpResult PalaceService::deletePayment(const string& id) {
    require(Action::ManagePayments);

    for (const Payment& payment : get_db().get_payments()) {
        if (payment.get_id() != id) continue;
        if (payment.get_status() == PaymentStatus::DaThanhToan)
            throw BusinessException("Khoản đã thanh toán không được xóa.");
    }

    vector<Payment>& payments = get_db().get_payments();
    payments.erase(remove_if(payments.begin(), payments.end(),
                             [&](const Payment& payment) { return payment.get_id() == id; }),
                   payments.end());
    return persist("Đã xóa khoản thanh toán.", id);
}

void PalaceService::applyPaymentCompleted(const string& bookingId, PaymentType type) {
    Booking* booking = nullptr;
    for (Booking& item : get_db().get_bookings()) {
        if (item.get_id() == bookingId) {
            booking = &item;
            break;
        }
    }
    if (booking == nullptr || booking->get_status() == BookingStatus::DaHuy) return;

    if ((type == PaymentType::DatCoc1 || type == PaymentType::DatCoc2) &&
        booking->get_status() == BookingStatus::ChoCoc) {
        booking->set_status(BookingStatus::DaCoc);
    }
    if (type == PaymentType::TatToan && booking->get_status() != BookingStatus::HoanTat) {
        booking->set_status(BookingStatus::HoanTat);
    }

    if (findEventByBooking(bookingId) != nullptr) return;

    string eventId = nextId("EVT-", eventIds());
    const EventRequest* request = findRequest(booking->get_requestId());
    get_db().get_events().push_back(Event(eventId, bookingId, booking->get_customerId(), booking->get_hallId(),
                                 booking->get_start(), booking->get_end(), EventStatus::SapDienRa,
                                 request != nullptr ? request->get_notes() : ""));
}

string PalaceService::paymentReceipt(const string& paymentId) const {
    const Payment* payment = findById(get_db().get_payments(), paymentId);
    if (payment == nullptr) return "Không tìm thấy phiếu thu.";

    const Booking* booking = findBooking(payment->get_bookingId());
    const Customer* customer = booking != nullptr ? findCustomer(booking->get_customerId()) : nullptr;
    const Hall* hall = booking != nullptr ? findHall(booking->get_hallId()) : nullptr;

    ostringstream output;
    output << "========== PHIẾU THU ==========\n";
    output << "Số phiếu     : " << payment->get_id() << '\n';
    output << "Booking      : " << payment->get_bookingId() << '\n';
    if (customer != nullptr)
        output << "Khách hàng   : " << customer->get_fullName() << " - " << customer->get_phone() << '\n';
    if (hall != nullptr) output << "Sảnh         : " << hall->get_name() << '\n';
    if (booking != nullptr) output << "Tổng booking : " << formatMoney(booking->get_finalTotal()) << '\n';
    output << "Khoản thu    : " << toString(payment->get_type()) << '\n';
    output << "Số tiền      : " << formatMoney(payment->get_amount()) << '\n';
    output << "Hình thức    : " << toString(payment->get_method()) << '\n';
    output << "Ngày thu     : " << payment->get_paidAt().toString() << '\n';
    output << "Trạng thái   : " << toString(payment->get_status()) << '\n';
    output << "Ghi chú      : " << payment->get_note() << '\n';
    output << "Người thu    : " << payment->get_recordedBy() << '\n';
    output << "===============================";
    return output.str();
}

// ==================== SU KIEN ====================

vector<Event> PalaceService::allEvents() const { return get_db().get_events(); }

const Event* PalaceService::findEventByBooking(const string& bookingId) const {
    for (const Event& event : get_db().get_events())
        if (event.get_bookingId() == bookingId) return &event;
    return nullptr;
}

vector<User> PalaceService::allUsers() const { return get_db().get_users(); }
vector<EventRequest> PalaceService::allRequests() const { return get_db().get_requests(); }
vector<Booking> PalaceService::allBookings() const { return get_db().get_bookings(); }
vector<Payment> PalaceService::allPayments() const { return get_db().get_payments(); }
vector<Hall> PalaceService::allHalls() const { return get_db().get_halls(); }

vector<const Service*> PalaceService::allServices() const {
    vector<const Service*> result;
    for (const Service* service : get_db().get_services()) result.push_back(service);
    return result;
}

int PalaceService::refreshEventStatuses(DateTime now) {
    int changed = 0;
    for (Event& event : get_db().get_events()) {
        if (event.get_status() == EventStatus::DaHuy) continue;
        EventStatus expected = eventTimeStatus(event.get_start(), event.get_end(), now);
        if (expected != event.get_status()) {
            event.set_status(expected);
            ++changed;
        }
    }
    return changed;
}

int PalaceService::sweepExpiredHolds(DateTime now) {
    int cancelled = 0;
    for (Booking& booking : get_db().get_bookings()) {
        if (booking.get_status() != BookingStatus::ChoCoc) continue;
        if (!(booking.get_holdUntil() < now)) continue;

        bool deposited = false;
        for (const Payment& payment : get_db().get_payments()) {
            if (payment.get_bookingId() == booking.get_id() &&
                payment.get_status() == PaymentStatus::DaThanhToan &&
                (payment.get_type() == PaymentType::DatCoc1 ||
                 payment.get_type() == PaymentType::DatCoc2)) {
                deposited = true;
                break;
            }
        }
        if (deposited) continue;

        booking.set_status(BookingStatus::DaHuy);
        booking.set_cancelReason("Quá hạn giữ chỗ " + to_string(kHoldHours) + " giờ chưa đặt cọc.");
        for (EventRequest& request : get_db().get_requests()) {
            if (request.get_id() == booking.get_requestId() && request.get_status() == RequestStatus::DaChot) {
                request.set_status(RequestStatus::HoanThanh);
                break;
            }
        }
        for (Event& event : get_db().get_events()) {
            if (event.get_bookingId() == booking.get_id()) event.set_status(EventStatus::DaHuy);
        }
        ++cancelled;
    }
    return cancelled;
}
