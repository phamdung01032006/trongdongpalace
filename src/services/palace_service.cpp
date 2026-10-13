#include "services/palace_service.hpp"

#include "utils/sha256.hpp"

using namespace std;

namespace tdp {

namespace {

bool roleAllows(Role role, Action action) {
    if (role == Role::Admin) return true;
    switch (action) {
        case Action::ManageUsers:
        case Action::ManageVenues:
            return false;
        case Action::ManageCustomers:
        case Action::ManageRequests:
        case Action::ManageBookings:
            return role == Role::Sales;
        case Action::ManagePayments:
            // Dac ta B.3: nhan vien kinh doanh ghi nhan cac khoan thu.
            return role == Role::Sales || role == Role::Accountant;
        case Action::ManageOperations:
            return role == Role::Coordinator;
        case Action::LookupHalls:
            return true;
    }
    return false;
}

}  // namespace

PalaceService::PalaceService(JsonStorage& storage) : storage_(storage) {}

const User* PalaceService::currentUser() const {
    if (currentUserId_.empty()) return nullptr;
    return findById(data().users, currentUserId_);
}

template <typename T>
T* PalaceService::findById(vector<T>& items, const string& id) {
    for (auto& item : items)
        if (item.id() == id) return &item;
    return nullptr;
}
template <typename T>
const T* PalaceService::findById(const vector<T>& items, const string& id) const {
    for (const auto& item : items)
        if (item.id() == id) return &item;
    return nullptr;
}

bool PalaceService::can(Action action) const {
    const User* u = currentUser();
    if (!u || !u->active()) return false;
    return roleAllows(u->role(), action);
}

OpResult PalaceService::require(Action action) const {
    const User* u = currentUser();
    if (!u) return OpResult::fail("Chưa đăng nhập.");
    if (!u->active()) return OpResult::fail("Tài khoản đã bị khóa.");
    if (!can(action)) return OpResult::fail("Bạn không có quyền thực hiện thao tác này.");
    return OpResult::success();
}

OpResult PalaceService::persist(const string& successMessage, const string& id) {
    try {
        storage_.save();
        return OpResult::success(successMessage, id);
    } catch (const exception& ex) {
        return OpResult::fail(string("Lưu dữ liệu thất bại: ") + ex.what());
    }
}

OpResult PalaceService::login(const string& username, const string& password) {
    for (const auto& user : data().users) {
        if (toLower(user.username()) != toLower(trim(username))) continue;
        if (!user.active()) return OpResult::fail("Tài khoản đã bị khóa.");
        if (!verifyPassword(password, user.salt(), user.passwordHash())) return OpResult::fail("Sai mật khẩu.");
        currentUserId_ = user.id();
        return OpResult::success("Xin chào " + user.fullName(), user.id());
    }
    return OpResult::fail("Không tìm thấy tài khoản.");
}

void PalaceService::logout() { currentUserId_.clear(); }

OpResult PalaceService::registerUser(const string& username, const string& password,
                                     const string& confirmPassword, const string& fullName, Role role) {
    if (trim(username).empty()) return OpResult::fail("Tên đăng nhập không được để trống.");
    if (password.size() < 4) return OpResult::fail("Mật khẩu tối thiểu 4 ký tự.");
    if (password != confirmPassword) return OpResult::fail("Mật khẩu nhập lại không khớp.");
    if (!uniqueUsername(trim(username), "")) return OpResult::fail("Tên đăng nhập đã tồn tại.");
    auto ids = userIds();
    string id = nextId("USR-", ids);
    string salt = randomSaltHex();
    User u(id, trim(username), salt, hashPassword(password, salt),
           trim(fullName).empty() ? trim(username) : trim(fullName), role, true);
    u.setDemoPassword(password);
    mut().users.push_back(move(u));
    return persist("Đã tạo tài khoản " + id + ". Bạn có thể đăng nhập ngay.", id);
}

bool PalaceService::uniqueUsername(const string& username, const string& exceptId) const {
    for (const auto& u : data().users)
        if (u.id() != exceptId && toLower(u.username()) == toLower(username)) return false;
    return true;
}
bool PalaceService::uniquePhone(const string& phone, const string& exceptId) const {
    for (const auto& c : data().customers)
        if (c.id() != exceptId && c.phone() == phone) return false;
    return true;
}

OpResult PalaceService::createUser(const string& username, const string& password, const string& fullName,
                                   Role role) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    if (trim(username).empty() || password.size() < 4 || trim(fullName).empty())
        return OpResult::fail("Tên đăng nhập, họ tên bắt buộc; mật khẩu tối thiểu 4 ký tự.");
    if (!uniqueUsername(trim(username), "")) return OpResult::fail("Tên đăng nhập đã tồn tại.");
    auto ids = userIds();
    string id = nextId("USR-", ids);
    string salt = randomSaltHex();
    User u(id, trim(username), salt, hashPassword(password, salt), trim(fullName), role, true);
    u.setDemoPassword(password);
    mut().users.push_back(move(u));
    return persist("Đã tạo tài khoản " + id, id);
}

OpResult PalaceService::setUserActive(const string& userId, bool active) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    auto* u = findById(mut().users, userId);
    if (!u) return OpResult::fail("Không tìm thấy tài khoản.");
    if (currentUserId_ == userId && !active) return OpResult::fail("Không thể tự khóa tài khoản đang dùng.");
    u->setActive(active);
    return persist(active ? "Đã mở khóa tài khoản." : "Đã khóa tài khoản.", userId);
}

OpResult PalaceService::changePassword(const string& userId, const string& newPassword) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    if (newPassword.size() < 4) return OpResult::fail("Mật khẩu tối thiểu 4 ký tự.");
    auto* u = findById(mut().users, userId);
    if (!u) return OpResult::fail("Không tìm thấy tài khoản.");
    string salt = randomSaltHex();
    u->setPassword(salt, hashPassword(newPassword, salt));
    u->setDemoPassword(newPassword);
    return persist("Đã đổi mật khẩu.", userId);
}

const Customer* PalaceService::findCustomer(const string& id) const { return findById(data().customers, id); }
const EventRequest* PalaceService::findRequest(const string& id) const { return findById(data().requests, id); }
const Hall* PalaceService::findHall(const string& id) const { return findById(data().halls, id); }
const ServiceItem* PalaceService::findService(const string& id) const { return findById(data().services, id); }
const Booking* PalaceService::findBooking(const string& id) const { return findById(data().bookings, id); }
const Event* PalaceService::findEvent(const string& id) const { return findById(data().events, id); }

const Event* PalaceService::findEventByBooking(const string& bookingId) const {
    for (const auto& e : data().events)
        if (e.bookingId() == bookingId) return &e;
    return nullptr;
}

vector<string> PalaceService::userIds() const {
    vector<string> ids;
    for (const auto& u : data().users) ids.push_back(u.id());
    return ids;
}
vector<string> PalaceService::customerIds() const {
    vector<string> ids;
    for (const auto& c : data().customers) ids.push_back(c.id());
    return ids;
}

OpResult PalaceService::addCustomer(const string& name, const string& phone, const string& email,
                                    const string& address, const string& notes) {
    auto r = require(Action::ManageCustomers);
    if (!r.ok) return r;
    if (trim(name).empty()) return OpResult::fail("Tên khách hàng là bắt buộc.");
    if (!looksLikePhone(trim(phone))) return OpResult::fail("Số điện thoại không hợp lệ (9-12 chữ số).");
    if (!looksLikeEmail(trim(email))) return OpResult::fail("Email không đúng định dạng.");
    if (!uniquePhone(trim(phone), "")) return OpResult::fail("Số điện thoại đã tồn tại.");
    string id = nextId("CUS-", customerIds());
    mut().customers.push_back(Customer(id, trim(name), trim(phone), trim(email), trim(address), trim(notes), DateTime::now()));
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

vector<Customer> PalaceService::searchCustomers(const string& keyword) const {
    vector<Customer> out;
    for (const auto& c : data().customers) {
        if (keyword.empty() || containsIgnoreCase(c.id(), keyword) || containsIgnoreCase(c.name(), keyword) ||
            containsIgnoreCase(c.phone(), keyword))
            out.push_back(c);
    }
    return out;
}

OpResult PalaceService::addRequest(const string& customerId, EventType type, const string& title, DateTime start,
                                   DateTime end, int guestCount, const string& special) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    if (!findCustomer(customerId)) return OpResult::fail("Khách hàng không tồn tại.");
    if (trim(title).empty()) return OpResult::fail("Tên/tiêu đề sự kiện là bắt buộc.");
    if (!start.valid() || !end.valid()) return OpResult::fail("Ngày giờ không hợp lệ.");
    if (!(start < end)) return OpResult::fail("Thời gian bắt đầu phải sớm hơn thời gian kết thúc.");
    if (guestCount <= 0) return OpResult::fail("Số lượng khách phải lớn hơn 0.");
    vector<string> ids;
    for (const auto& x : data().requests) ids.push_back(x.id());
    string id = nextId("REQ-", ids);
    mut().requests.push_back(EventRequest(id, customerId, type, trim(title), start, end, guestCount, trim(special),
                                          RequestStatus::TiepNhan));
    return persist("Đã tiếp nhận yêu cầu " + id, id);
}

OpResult PalaceService::updateRequestStatus(const string& id, RequestStatus status) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, id);
    if (!req) return OpResult::fail("Không tìm thấy yêu cầu.");
    if (req->status() == RequestStatus::DaChuyenBooking && status != RequestStatus::DaChuyenBooking)
        return OpResult::fail("Yêu cầu đã chuyển thành booking, không đổi trạng thái này.");
    req->setStatus(status);
    return persist("Đã cập nhật trạng thái yêu cầu.", id);
}


OpResult PalaceService::addHall(const string& name, int capacity) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (trim(name).empty()) return OpResult::fail("Tên sảnh là bắt buộc.");
    if (capacity <= 0) return OpResult::fail("Sức chứa phải lớn hơn 0.");
    vector<string> ids;
    for (const auto& x : data().halls) ids.push_back(x.id());
    string id = nextId("HAL-", ids);
    mut().halls.push_back(Hall(id, trim(name), capacity, true));
    return persist("Đã thêm sảnh.", id);
}
OpResult PalaceService::updateHall(const string& id, const string& name, int capacity, bool active) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* h = findById(mut().halls, id);
    if (!h) return OpResult::fail("Không tìm thấy sảnh.");
    if (trim(name).empty() || capacity <= 0) return OpResult::fail("Tên sảnh và sức chứa không hợp lệ.");
    h->setName(trim(name));
    h->setCapacity(capacity);
    h->setActive(active);
    return persist("Đã cập nhật sảnh.", id);
}

OpResult PalaceService::addService(const string& name, const string& unit, int64_t price,
                                   const string& category) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (trim(name).empty() || price < 0) return OpResult::fail("Tên dịch vụ bắt buộc, đơn giá không âm.");
    vector<string> ids;
    for (const auto& x : data().services) ids.push_back(x.id());
    string id = nextId("SVC-", ids);
    mut().services.push_back(ServiceItem(id, trim(name), trim(unit).empty() ? "lần" : trim(unit), price,
                                         trim(category).empty() ? "Khac" : trim(category), true));
    return persist("Đã thêm dịch vụ.", id);
}
OpResult PalaceService::updateService(const string& id, const string& name, const string& unit, int64_t price,
                                      const string& category, bool active) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* s = findById(mut().services, id);
    if (!s) return OpResult::fail("Không tìm thấy dịch vụ.");
    if (trim(name).empty() || price < 0) return OpResult::fail("Dữ liệu dịch vụ không hợp lệ.");
    s->setName(trim(name));
    s->setUnit(trim(unit));
    s->setUnitPrice(price);
    s->setCategory(trim(category));
    s->setActive(active);
    return persist("Đã cập nhật dịch vụ (lịch sử báo giá cũ vẫn giữ đơn giá đã chốt).", id);
}

bool PalaceService::hallAvailable(const string& hallId, DateTime start, DateTime end,
                                  const string& ignoreBookingId, string& reason) const {
    const Hall* hall = findHall(hallId);
    if (!hall) {
        reason = "Sảnh không tồn tại.";
        return false;
    }
    if (!hall->active()) {
        reason = "Sảnh đang ngừng hoạt động.";
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






OpResult PalaceService::createBooking(const string& requestId, const string& hallId, DateTime start,
                                      DateTime end, int guestCount) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, requestId);
    if (!req) return OpResult::fail("Yêu cầu sự kiện không tồn tại.");
    if (req->status() == RequestStatus::Huy || req->status() == RequestStatus::TuChoi)
        return OpResult::fail("Không tạo booking từ yêu cầu đã hủy/từ chối.");
    if (!findCustomer(req->customerId())) return OpResult::fail("Khách hàng không tồn tại.");
    const Hall* hall = findHall(hallId);
    if (!hall) return OpResult::fail("Sảnh không tồn tại.");
    if (guestCount <= 0) return OpResult::fail("Số khách phải lớn hơn 0.");
    if (guestCount > hall->capacity()) return OpResult::fail("Số khách vượt sức chứa sảnh.");
    string reason;
    if (!hallAvailable(hallId, start, end, "", reason)) return OpResult::fail(reason);
    vector<string> ids;
    for (const auto& x : data().bookings) ids.push_back(x.id());
    string id = nextId("BKG-", ids);
    mut().bookings.push_back(Booking(id, requestId, req->customerId(), hallId, start, end, guestCount,
                                     BookingStatus::Nhat));
    req->setStatus(RequestStatus::DaChuyenBooking);
    return persist("Đã tạo booking " + id + " từ yêu cầu " + requestId, id);
}


OpResult PalaceService::confirmBooking(const string& bookingId) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Không xác nhận booking đã hủy.");
    if (b->status() == BookingStatus::DaXacNhan) return OpResult::fail("Booking đã được xác nhận.");
    if (!findCustomer(b->customerId()) || !findRequest(b->requestId()))
        return OpResult::fail("Booking thiếu liên kết khách hàng/yêu cầu hợp lệ.");
    const Hall* hall = findHall(b->hallId());
    if (!hall) return OpResult::fail("Sảnh không tồn tại.");
    if (b->guestCount() > hall->capacity()) return OpResult::fail("Số khách vượt sức chứa.");
    string reason;
    if (!hallAvailable(b->hallId(), b->start(), b->end(), b->id(), reason)) return OpResult::fail(reason);
    b->setStatus(BookingStatus::DaXacNhan);
    if (auto* req = findById(mut().requests, b->requestId())) req->setStatus(RequestStatus::DaChuyenBooking);
    if (!findEventByBooking(b->id())) {
        vector<string> ids;
        for (const auto& x : data().events) ids.push_back(x.id());
        string eid = nextId("EVT-", ids);
        mut().events.push_back(Event(eid, b->id(), b->customerId(), b->hallId(), b->start(), b->end(),
                                     EventStatus::ChuanBi, findRequest(b->requestId())->specialRequests()));
    }
    return persist("Đã xác nhận đặt sảnh " + bookingId, bookingId);
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


int64_t PalaceService::paidAmount(const string& bookingId) const {
    int64_t sum = 0;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId && p.amount() > 0) sum += p.amount();
    return sum;
}vector<Payment> PalaceService::paymentsOf(const string& bookingId) const {
    vector<Payment> out;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId) out.push_back(p);
    return out;
}

OpResult PalaceService::recordPayment(const string& bookingId, int64_t amount, PaymentMethod method,
                                      const string& note) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    const Booking* b = findBooking(bookingId);
    if (!b) return OpResult::fail("Booking không tồn tại.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Không thu tiền cho booking đã hủy.");
    if (amount <= 0) return OpResult::fail("Số tiền thanh toán phải lớn hơn 0.");
    if (trim(note).empty()) return OpResult::fail("Ghi chú thanh toán là bắt buộc.");
    vector<string> ids;
    for (const auto& x : data().payments) ids.push_back(x.id());
    string id = nextId("PAY-", ids);
    const User* actor = currentUser();
    mut().payments.push_back(Payment(id, bookingId, amount, DateTime::now(), method, trim(note),
                                     actor ? actor->username() : ""));
    return persist("Đã ghi nhận thanh toán " + formatMoney(amount) + " cho booking " + bookingId, id);
}


OpResult PalaceService::updateEventStatus(const string& eventId, EventStatus status) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    if (status == EventStatus::HoanTat) return completeEvent(eventId);
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    e->setStatus(status);
    return persist("Đã cập nhật trạng thái sự kiện.", eventId);
}

OpResult PalaceService::completeEvent(const string& eventId) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    if (e->status() == EventStatus::Huy) return OpResult::fail("Sự kiện đã hủy.");
    e->setStatus(EventStatus::HoanTat);
    return persist("Đã xác nhận hoàn tất sự kiện.", eventId);
}

OpResult PalaceService::setEventNotes(const string& eventId, const string& notes) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    e->setSpecialNotes(trim(notes));
    return persist("Đã cập nhật ghi chú sự kiện.", eventId);
}

}  // namespace tdp
