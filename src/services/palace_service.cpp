#include "services/palace_service.hpp"

#include "utils/sha256.hpp"

using namespace std;

namespace tdp {

namespace {

bool roleAllows(Role role, Action action) {
    if (role == Role::Admin) return true;
    switch (action) {
        case Action::ManageUsers:
        case Action::ViewAudit:
            return false;
        case Action::ManageVenues:
            return false;
        case Action::ManageCustomers:
        case Action::ManageRequests:
        case Action::ManageQuotations:
        case Action::ManageBookings:
            return role == Role::Sales;
        case Action::ManageContracts:
            return role == Role::Accountant || role == Role::Sales;
        case Action::ManagePayments:
            return role == Role::Accountant;
        case Action::ManageOperations:
            return role == Role::Coordinator;
        case Action::LookupHalls:
        case Action::ViewReports:
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

void PalaceService::audit(const string& action, const string& detail) {
    auto ids = vector<string>{};
    for (const auto& log : mut().logs) ids.push_back(log.id());
    const User* u = currentUser();
    mut().logs.push_back(AuditLog(nextId("LOG-", ids), DateTime::now(), u ? u->username() : "-", action, detail));
}

OpResult PalaceService::persist(const string& successMessage, const string& id, const string& auditAction,
                                const string& auditDetail) {
    try {
        if (!auditAction.empty()) audit(auditAction, auditDetail);
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
        audit("DANG_NHAP", "Đăng nhập thành công");
        try {
            storage_.save();
        } catch (const exception& ex) {
            currentUserId_.clear();
            return OpResult::fail(string("Không ghi được nhật ký đăng nhập: ") + ex.what());
        }
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
    return persist("Đã tạo tài khoản " + id + ". Bạn có thể đăng nhập ngay.", id, "DANG_KY", id);
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
    return persist("Đã tạo tài khoản " + id, id, "TAO_TAI_KHOAN", id);
}

OpResult PalaceService::setUserActive(const string& userId, bool active) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    auto* u = findById(mut().users, userId);
    if (!u) return OpResult::fail("Không tìm thấy tài khoản.");
    if (currentUserId_ == userId && !active) return OpResult::fail("Không thể tự khóa tài khoản đang dùng.");
    u->setActive(active);
    return persist(active ? "Đã mở khóa tài khoản." : "Đã khóa tài khoản.", userId, "CAP_NHAT_TAI_KHOAN", userId);
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
    return persist("Đã đổi mật khẩu.", userId, "DOI_MAT_KHAU", userId);
}

const Customer* PalaceService::findCustomer(const string& id) const { return findById(data().customers, id); }
const EventRequest* PalaceService::findRequest(const string& id) const { return findById(data().requests, id); }
const Hall* PalaceService::findHall(const string& id) const { return findById(data().halls, id); }
const ServiceItem* PalaceService::findService(const string& id) const { return findById(data().services, id); }
const Quotation* PalaceService::findQuotation(const string& id) const { return findById(data().quotations, id); }
const Booking* PalaceService::findBooking(const string& id) const { return findById(data().bookings, id); }
const Contract* PalaceService::findContract(const string& id) const { return findById(data().contracts, id); }
const Event* PalaceService::findEvent(const string& id) const { return findById(data().events, id); }

const Contract* PalaceService::findContractByBooking(const string& bookingId) const {
    for (const auto& c : data().contracts)
        if (c.bookingId() == bookingId) return &c;
    return nullptr;
}
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
    return persist("Đã thêm khách hàng " + id, id, "THEM_KHACH_HANG", id);
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
    return persist("Đã cập nhật khách hàng.", id, "SUA_KHACH_HANG", id);
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
    return persist("Đã tiếp nhận yêu cầu " + id, id, "TIEP_NHAN_YEU_CAU", id);
}

OpResult PalaceService::updateRequestStatus(const string& id, RequestStatus status) {
    auto r = require(Action::ManageRequests);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, id);
    if (!req) return OpResult::fail("Không tìm thấy yêu cầu.");
    if (req->status() == RequestStatus::DaChuyenBooking && status != RequestStatus::DaChuyenBooking)
        return OpResult::fail("Yêu cầu đã chuyển thành booking, không đổi trạng thái này.");
    req->setStatus(status);
    return persist("Đã cập nhật trạng thái yêu cầu.", id, "CAP_NHAT_YEU_CAU", id);
}

OpResult PalaceService::addCenter(const string& name, const string& address) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    if (trim(name).empty()) return OpResult::fail("Tên trung tâm là bắt buộc.");
    vector<string> ids;
    for (const auto& x : data().centers) ids.push_back(x.id());
    string id = nextId("CTR-", ids);
    mut().centers.push_back(Center(id, trim(name), trim(address), true));
    return persist("Đã thêm trung tâm.", id, "THEM_TRUNG_TAM", id);
}
OpResult PalaceService::updateCenter(const string& id, const string& name, const string& address, bool active) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* c = findById(mut().centers, id);
    if (!c) return OpResult::fail("Không tìm thấy trung tâm.");
    if (trim(name).empty()) return OpResult::fail("Tên trung tâm là bắt buộc.");
    c->setName(trim(name));
    c->setAddress(trim(address));
    c->setActive(active);
    return persist("Đã cập nhật trung tâm.", id, "", "");
}

OpResult PalaceService::addHall(const string& centerId, const string& name, int capacity) {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    auto* center = findById(mut().centers, centerId);
    if (!center) return OpResult::fail("Trung tâm không tồn tại.");
    if (trim(name).empty()) return OpResult::fail("Tên sảnh là bắt buộc.");
    if (capacity <= 0) return OpResult::fail("Sức chứa phải lớn hơn 0.");
    vector<string> ids;
    for (const auto& x : data().halls) ids.push_back(x.id());
    string id = nextId("HAL-", ids);
    mut().halls.push_back(Hall(id, centerId, trim(name), capacity, true));
    return persist("Đã thêm sảnh.", id, "THEM_SANH", id);
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
    return persist("Đã cập nhật sảnh.", id, "", "");
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
    return persist("Đã thêm dịch vụ.", id, "THEM_DICH_VU", id);
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
    return persist("Đã cập nhật dịch vụ (lịch sử báo giá cũ vẫn giữ đơn giá đã chốt).", id, "", "");
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
    const Center* center = findById(data().centers, hall->centerId());
    if (!center || !center->active()) {
        reason = "Trung tâm của sảnh đang ngừng hoạt động.";
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

OpResult PalaceService::createQuotation(const string& requestId, const string& note) {
    auto r = require(Action::ManageQuotations);
    if (!r.ok) return r;
    auto* req = findById(mut().requests, requestId);
    if (!req) return OpResult::fail("Yêu cầu sự kiện không tồn tại.");
    if (req->status() == RequestStatus::Huy || req->status() == RequestStatus::TuChoi)
        return OpResult::fail("Không tạo báo giá cho yêu cầu đã hủy/từ chối.");
    if (!findCustomer(req->customerId())) return OpResult::fail("Khách hàng của yêu cầu không tồn tại.");
    vector<string> ids;
    for (const auto& x : data().quotations) ids.push_back(x.id());
    string id = nextId("QUO-", ids);
    mut().quotations.push_back(Quotation(id, requestId, req->customerId(), trim(note)));
    if (req->status() == RequestStatus::TiepNhan) req->setStatus(RequestStatus::DaBaoGia);
    return persist("Đã tạo báo giá " + id, id, "TAO_BAO_GIA", id);
}

OpResult PalaceService::addQuotationLine(const string& quotationId, const string& serviceId, int quantity) {
    auto r = require(Action::ManageQuotations);
    if (!r.ok) return r;
    auto* q = findById(mut().quotations, quotationId);
    if (!q) return OpResult::fail("Không tìm thấy báo giá.");
    if (q->status() == QuotationStatus::DaChapNhan || q->status() == QuotationStatus::TuChoi)
        return OpResult::fail("Không sửa báo giá đã chấp nhận hoặc từ chối.");
    const ServiceItem* svc = findService(serviceId);
    if (!svc) return OpResult::fail("Dịch vụ không tồn tại.");
    if (!svc->active()) return OpResult::fail("Chỉ được thêm dịch vụ đang hoạt động vào báo giá mới.");
    if (quantity <= 0) return OpResult::fail("Số lượng phải lớn hơn 0.");
    q->lines().push_back(QuotationLine(svc->id(), svc->name(), quantity, svc->unitPrice()));
    q->recalc();
    return persist("Đã thêm dòng dịch vụ. Tổng: " + formatMoney(q->total()), quotationId, "", "");
}

OpResult PalaceService::updateQuotationLine(const string& quotationId, int index, int quantity) {
    auto r = require(Action::ManageQuotations);
    if (!r.ok) return r;
    auto* q = findById(mut().quotations, quotationId);
    if (!q) return OpResult::fail("Không tìm thấy báo giá.");
    if (q->status() == QuotationStatus::DaChapNhan || q->status() == QuotationStatus::TuChoi)
        return OpResult::fail("Không sửa báo giá đã khóa.");
    if (index < 0 || index >= static_cast<int>(q->lines().size())) return OpResult::fail("Dòng không tồn tại.");
    if (quantity <= 0) return OpResult::fail("Số lượng phải lớn hơn 0.");
    q->lines()[static_cast<size_t>(index)].setQuantity(quantity);
    q->recalc();
    return persist("Đã cập nhật dòng. Tổng: " + formatMoney(q->total()), quotationId, "", "");
}

OpResult PalaceService::removeQuotationLine(const string& quotationId, int index) {
    auto r = require(Action::ManageQuotations);
    if (!r.ok) return r;
    auto* q = findById(mut().quotations, quotationId);
    if (!q) return OpResult::fail("Không tìm thấy báo giá.");
    if (q->status() == QuotationStatus::DaChapNhan || q->status() == QuotationStatus::TuChoi)
        return OpResult::fail("Không sửa báo giá đã khóa.");
    if (index < 0 || index >= static_cast<int>(q->lines().size())) return OpResult::fail("Dòng không tồn tại.");
    q->lines().erase(q->lines().begin() + index);
    q->recalc();
    return persist("Đã xóa dòng. Tổng: " + formatMoney(q->total()), quotationId, "", "");
}

OpResult PalaceService::setQuotationStatus(const string& quotationId, QuotationStatus status) {
    auto r = require(Action::ManageQuotations);
    if (!r.ok) return r;
    auto* q = findById(mut().quotations, quotationId);
    if (!q) return OpResult::fail("Không tìm thấy báo giá.");
    if (status == QuotationStatus::DaChapNhan && q->lines().empty())
        return OpResult::fail("Không chấp nhận báo giá trống.");
    q->recalc();
    q->setStatus(status);
    return persist("Đã cập nhật trạng thái báo giá.", quotationId, "CAP_NHAT_BAO_GIA", quotationId);
}

OpResult PalaceService::createBooking(const string& quotationId, const string& hallId, DateTime start,
                                      DateTime end, int guestCount) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* q = findById(mut().quotations, quotationId);
    if (!q) return OpResult::fail("Báo giá không tồn tại.");
    if (q->status() == QuotationStatus::TuChoi)
        return OpResult::fail("Không tạo booking từ báo giá đã bị từ chối.");
    if (!findCustomer(q->customerId())) return OpResult::fail("Khách hàng không tồn tại.");
    auto* req = findById(mut().requests, q->requestId());
    if (!req) return OpResult::fail("Yêu cầu sự kiện không tồn tại.");
    const Hall* hall = findHall(hallId);
    if (!hall) return OpResult::fail("Sảnh không tồn tại.");
    if (guestCount <= 0) return OpResult::fail("Số khách phải lớn hơn 0.");
    if (guestCount > hall->capacity()) return OpResult::fail("Số khách vượt sức chứa sảnh.");
    string reason;
    if (!hallAvailable(hallId, start, end, "", reason)) return OpResult::fail(reason);
    vector<string> ids;
    for (const auto& x : data().bookings) ids.push_back(x.id());
    string id = nextId("BKG-", ids);
    mut().bookings.push_back(Booking(id, quotationId, q->requestId(), q->customerId(), hallId, start, end, guestCount,
                                     BookingStatus::Nhat));
    return persist("Đã tạo booking nháp " + id, id, "TAO_BOOKING", id);
}

OpResult PalaceService::confirmBooking(const string& bookingId) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Không xác nhận booking đã hủy.");
    if (b->status() == BookingStatus::DaXacNhan) return OpResult::fail("Booking đã được xác nhận.");
    auto* q = findById(mut().quotations, b->quotationId());
    if (!findCustomer(b->customerId()) || !findRequest(b->requestId()) || !q)
        return OpResult::fail("Booking thiếu liên kết khách hàng/sự kiện/báo giá hợp lệ.");
    if (q->status() == QuotationStatus::TuChoi) return OpResult::fail("Báo giá đã bị từ chối.");
    if (q->status() != QuotationStatus::DaChapNhan) q->setStatus(QuotationStatus::DaChapNhan);
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
        mut().events.push_back(Event(eid, b->id(), "", b->customerId(), b->hallId(), b->start(), b->end(),
                                     EventStatus::ChuanBi, findRequest(b->requestId())->specialRequests()));
    }
    return persist("Đã xác nhận đặt sảnh " + bookingId, bookingId, "XAC_NHAN_BOOKING", bookingId);
}

OpResult PalaceService::cancelBooking(const string& bookingId, const string& reason) {
    auto r = require(Action::ManageBookings);
    if (!r.ok) return r;
    auto* b = findById(mut().bookings, bookingId);
    if (!b) return OpResult::fail("Không tìm thấy booking.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Booking đã hủy.");
    for (auto& c : mut().contracts) {
        if (c.bookingId() == bookingId && c.status() == ContractStatus::HieuLuc) {
            c.setStatus(ContractStatus::Huy);
        }
    }
    b->setStatus(BookingStatus::DaHuy);
    b->setCancelReason(trim(reason));
    for (auto& ev : mut().events) {
        if (ev.bookingId() == bookingId) ev.setStatus(EventStatus::Huy);
    }
    return persist("Đã hủy booking. Sảnh được giải phóng lịch.", bookingId, "HUY_BOOKING", bookingId + " | " + reason);
}

OpResult PalaceService::createContract(const string& bookingId) {
    auto r = require(Action::ManageContracts);
    if (!r.ok) return r;
    const Booking* b = findBooking(bookingId);
    if (!b) return OpResult::fail("Booking không tồn tại.");
    if (b->status() != BookingStatus::DaXacNhan)
        return OpResult::fail("Chỉ lập hợp đồng từ booking đã xác nhận và còn hiệu lực.");
    if (findContractByBooking(bookingId)) return OpResult::fail("Booking này đã có hợp đồng.");
    const Quotation* q = findQuotation(b->quotationId());
    if (!q || q->status() != QuotationStatus::DaChapNhan) return OpResult::fail("Báo giá không hợp lệ.");
    if (!findCustomer(b->customerId())) return OpResult::fail("Khách hàng không tồn tại.");
    vector<string> ids;
    for (const auto& x : data().contracts) ids.push_back(x.id());
    string id = nextId("CTD-", ids);
    string no = "HD-" + id.substr(4);
    mut().contracts.push_back(Contract(id, bookingId, b->customerId(), no, DateTime::now(), q->total(), ContractStatus::HieuLuc));
    if (auto* ev = findById(mut().events, findEventByBooking(bookingId) ? findEventByBooking(bookingId)->id() : ""))
        ev->setContractId(id);
    return persist("Đã lập hợp đồng " + no, id, "LAP_HOP_DONG", id);
}

int64_t PalaceService::paidAmount(const string& contractId) const {
    int64_t sum = 0;
    for (const auto& p : data().payments)
        if (p.contractId() == contractId && p.amount() > 0) sum += p.amount();
    return sum;
}
int64_t PalaceService::outstanding(const string& contractId) const {
    const Contract* c = findContract(contractId);
    if (!c) return 0;
    return c->totalAmount() - paidAmount(contractId);
}
vector<Payment> PalaceService::paymentsOf(const string& contractId) const {
    vector<Payment> out;
    for (const auto& p : data().payments)
        if (p.contractId() == contractId) out.push_back(p);
    return out;
}

OpResult PalaceService::recordPayment(const string& contractId, int64_t amount, PaymentMethod method,
                                      const string& note) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    const Contract* c = findContract(contractId);
    if (!c) return OpResult::fail("Hợp đồng không tồn tại.");
    if (c->status() == ContractStatus::Huy) return OpResult::fail("Không thu tiền cho hợp đồng đã hủy.");
    if (amount <= 0) return OpResult::fail("Số tiền thanh toán phải lớn hơn 0.");
    int64_t remain = outstanding(contractId);
    if (amount > remain)
        return OpResult::fail("Số tiền vượt số dư còn phải thu (" + formatMoney(remain) + ").");
    vector<string> ids;
    for (const auto& x : data().payments) ids.push_back(x.id());
    string id = nextId("PAY-", ids);
    const User* actor = currentUser();
    mut().payments.push_back(Payment(id, contractId, amount, DateTime::now(), method, trim(note),
                                     actor ? actor->username() : ""));
    return persist("Đã ghi nhận thanh toán " + formatMoney(amount) + ". Còn phải thu: " +
                       formatMoney(outstanding(contractId)),
                   id, "GHI_NHAN_THANH_TOAN", id);
}

OpResult PalaceService::addTask(const string& eventId, const string& title, const string& assignee,
                                DateTime due) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    if (!findEvent(eventId)) return OpResult::fail("Sự kiện vận hành không tồn tại.");
    if (trim(title).empty()) return OpResult::fail("Tên nhiệm vụ là bắt buộc.");
    vector<string> ids;
    for (const auto& x : data().tasks) ids.push_back(x.id());
    string id = nextId("TSK-", ids);
    mut().tasks.push_back(OperationTask(id, eventId, trim(title), trim(assignee), due, TaskStatus::CanLam, ""));
    return persist("Đã giao nhiệm vụ.", id, "GIAO_NHIEM_VU", id);
}

OpResult PalaceService::updateTaskStatus(const string& taskId, TaskStatus status, const string& note) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* t = findById(mut().tasks, taskId);
    if (!t) return OpResult::fail("Không tìm thấy nhiệm vụ.");
    t->setStatus(status);
    if (!trim(note).empty()) t->setNote(trim(note));
    return persist("Đã cập nhật nhiệm vụ.", taskId, "", "");
}

OpResult PalaceService::addIncident(const string& eventId, const string& description) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    if (!findEvent(eventId)) return OpResult::fail("Sự kiện không tồn tại.");
    if (trim(description).empty()) return OpResult::fail("Mô tả sự cố là bắt buộc.");
    vector<string> ids;
    for (const auto& x : data().incidents) ids.push_back(x.id());
    string id = nextId("INC-", ids);
    mut().incidents.push_back(Incident(id, eventId, trim(description), IncidentStatus::Mo, "", DateTime::now()));
    return persist("Đã ghi nhận sự cố.", id, "GHI_NHAN_SU_CO", id);
}

OpResult PalaceService::resolveIncident(const string& incidentId, const string& resolution,
                                        IncidentStatus status) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* inc = findById(mut().incidents, incidentId);
    if (!inc) return OpResult::fail("Không tìm thấy sự cố.");
    if (status == IncidentStatus::DaXuLy && trim(resolution).empty())
        return OpResult::fail("Cần ghi kết quả xử lý khi đóng sự cố.");
    inc->setStatus(status);
    inc->setResolution(trim(resolution));
    return persist("Đã cập nhật sự cố.", incidentId, "XU_LY_SU_CO", incidentId);
}

OpResult PalaceService::updateEventStatus(const string& eventId, EventStatus status) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    if (status == EventStatus::HoanTat) return completeEvent(eventId);
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    e->setStatus(status);
    return persist("Đã cập nhật trạng thái sự kiện.", eventId, "CAP_NHAT_SU_KIEN", eventId);
}

OpResult PalaceService::completeEvent(const string& eventId) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    if (e->status() == EventStatus::Huy) return OpResult::fail("Sự kiện đã hủy.");
    for (const auto& inc : data().incidents) {
        if (inc.eventId() == eventId && inc.status() != IncidentStatus::DaXuLy)
            return OpResult::fail("Không hoàn tất khi còn sự cố chưa xử lý xong (" + inc.id() + ").");
    }
    e->setStatus(EventStatus::HoanTat);
    if (auto* c = findById(mut().contracts, e->contractId())) c->setStatus(ContractStatus::HoanTat);
    return persist("Đã xác nhận hoàn tất sự kiện.", eventId, "HOAN_TAT_SU_KIEN", eventId);
}

OpResult PalaceService::setEventNotes(const string& eventId, const string& notes) {
    auto r = require(Action::ManageOperations);
    if (!r.ok) return r;
    auto* e = findById(mut().events, eventId);
    if (!e) return OpResult::fail("Không tìm thấy sự kiện.");
    e->setSpecialNotes(trim(notes));
    return persist("Đã cập nhật ghi chú sự kiện.", eventId, "", "");
}

vector<ReportRow> PalaceService::reportEventsByDateStatus(const string& fromText, const string& toText,
                                                               const string& statusFilter) const {
    vector<ReportRow> rows;
    auto from = DateTime::parse(fromText);
    auto to = DateTime::parse(toText);
    for (const auto& e : data().events) {
        if (from && e.start() < *from) continue;
        if (to && *to < e.start()) continue;
        if (!statusFilter.empty() && toString(e.status()) != statusFilter) continue;
        const Customer* c = findCustomer(e.customerId());
        const Hall* h = findHall(e.hallId());
        rows.push_back(ReportRow{{e.id(), e.start().toString(), c ? c->name() : e.customerId(),
                                  h ? h->name() : e.hallId(), toString(e.status())}});
    }
    return rows;
}

vector<ReportRow> PalaceService::reportUpcoming() const {
    DateTime now = DateTime::now();
    vector<ReportRow> rows;
    for (const auto& e : data().events) {
        if (e.status() == EventStatus::Huy || e.status() == EventStatus::HoanTat) continue;
        if (e.start() < now) continue;
        const Customer* c = findCustomer(e.customerId());
        rows.push_back(ReportRow{{e.id(), e.start().toString(), c ? c->name() : "", toString(e.status())}});
    }
    return rows;
}

vector<ReportRow> PalaceService::reportHallUsage() const {
    vector<ReportRow> rows;
    for (const auto& h : data().halls) {
        int active = 0, cancelled = 0;
        for (const auto& b : data().bookings) {
            if (b.hallId() != h.id()) continue;
            if (b.occupiesHall()) ++active;
            else ++cancelled;
        }
        rows.push_back(ReportRow{{h.id(), h.name(), to_string(h.capacity()), h.active() ? "Hoạt động" : "Ngừng",
                                  to_string(active), to_string(cancelled)}});
    }
    return rows;
}

vector<ReportRow> PalaceService::reportFinance() const {
    vector<ReportRow> rows;
    int64_t total = 0, paid = 0;
    for (const auto& c : data().contracts) {
        int64_t p = paidAmount(c.id());
        total += c.totalAmount();
        paid += p;
        rows.push_back(ReportRow{{c.contractNo(), formatMoney(c.totalAmount()), formatMoney(p), formatMoney(c.totalAmount() - p),
                                  toString(c.status())}});
    }
    rows.insert(rows.begin(), ReportRow{{"TỔNG", formatMoney(total), formatMoney(paid), formatMoney(total - paid), ""}});
    return rows;
}

vector<ReportRow> PalaceService::reportDebt() const {
    vector<ReportRow> rows;
    for (const auto& c : data().contracts) {
        int64_t remain = c.totalAmount() - paidAmount(c.id());
        if (remain <= 0) continue;
        const Customer* cus = findCustomer(c.customerId());
        rows.push_back(ReportRow{{c.contractNo(), cus ? cus->name() : "", formatMoney(remain)}});
    }
    return rows;
}

vector<ReportRow> PalaceService::reportOps() const {
    vector<ReportRow> rows;
    for (const auto& e : data().events) {
        int todo = 0, doing = 0, done = 0, openInc = 0;
        for (const auto& t : data().tasks) {
            if (t.eventId() != e.id()) continue;
            if (t.status() == TaskStatus::CanLam) ++todo;
            else if (t.status() == TaskStatus::DangLam) ++doing;
            else ++done;
        }
        for (const auto& i : data().incidents)
            if (i.eventId() == e.id() && i.status() != IncidentStatus::DaXuLy) ++openInc;
        rows.push_back(ReportRow{{e.id(), toString(e.status()), to_string(todo), to_string(doing),
                                  to_string(done), to_string(openInc)}});
    }
    return rows;
}

}  // namespace tdp
