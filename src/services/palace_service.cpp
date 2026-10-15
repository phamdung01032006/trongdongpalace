#ifndef TDP_SERVICES_PALACE_SERVICE_CPP
#define TDP_SERVICES_PALACE_SERVICE_CPP

#include "storage/text_storage.cpp"

#include <optional>
#include <string>
#include <vector>

using namespace std;

enum class Action {
    SubmitRequest,
    ManageUsers,
    ManageCustomers,
    ManageRequests,
    ManageVenues,
    LookupHalls,
    ManageBookings,
    ManagePayments,
    ManageEvents
};

// So gio giu cho sanh cho khach dat coc (dac ta B.2: 72 gio).
constexpr int kHoldHours = 72;
// Nguong chiet khau can Quan ly duyet (Form 2).
constexpr int kDiscountApprovalPercent = 10;

struct ServiceLineInput {
    string serviceId;
    int quantity = 0;  // <= 0: tu tinh theo don vi (ban/khach/goi)
};

class PalaceService {
public:
    explicit PalaceService(TextStorage& storage);

    OpResult login(const string& username, const string& password);
    // Tu dang ky tai khoan Customer: nhap du ho so -> tu tao Customer lien ket.
    OpResult registerUser(const string& username, const string& password, const string& confirmPassword,
                          const string& fullName, const string& phone, const string& email,
                          const string& address);
    void logout();
    bool isLoggedIn() const { return !currentUserId_.empty(); }
    const User* currentUser() const;
    bool can(Action action) const;
    OpResult require(Action action) const;

    const Database& db() const { return storage_.db(); }

    OpResult createUser(const string& username, const string& password, const string& fullName, Role role);
    OpResult updateUserRole(const string& userId, Role role);
    OpResult deleteUser(const string& userId);
    OpResult setUserActive(const string& userId, bool active);
    OpResult changePassword(const string& userId, const string& newPassword);

    OpResult addCustomer(const string& name, const string& phone, const string& email,
                         const string& address, const string& notes);
    OpResult updateCustomer(const string& id, const string& name, const string& phone,
                            const string& email, const string& address, const string& notes);
    OpResult deleteCustomer(const string& id);
    vector<Customer> searchCustomers(const string& keyword) const;
    const Customer* findCustomer(const string& id) const;

    // Form 1: tao yeu cau (khach hang hoac sales nhap ho).
    OpResult addRequest(const string& customerId, EventType type, const string& title,
                        const string& customerName, const string& customerPhone, const string& hallId,
                        DateTime start, DateTime end, int guestCount, const string& notes,
                        const vector<ServiceLineInput>& services, int customerAge = 0);
    // Sales cap nhat toan bo sau trao doi (dac ta B.1).
    OpResult updateRequest(const string& id, EventType type, const string& title,
                           const string& customerName, const string& customerPhone, const string& hallId,
                           DateTime start, DateTime end, int guestCount, const string& notes,
                           const vector<ServiceLineInput>& services);
    OpResult updateRequestStatus(const string& id, RequestStatus status);
    // Khach hang tu huy yeu cau cua minh.
    OpResult cancelOwnRequest(const string& id);
    OpResult deleteRequest(const string& id);
    const EventRequest* findRequest(const string& id) const;
    // Chi phi du kien = tien thue sanh + tong dich vu (dac ta A.2).
    int64_t estimateRequest(const EventRequest& req) const;
    // Lich su 10 yeu cau gan nhat da huy/hoan thanh/chot (dac ta B.1).
    vector<EventRequest> requestHistory(size_t limit = 10) const;
    // So yeu cau moi (Cho) de bao sales (dac ta B.1).
    size_t countNewRequests() const;
    vector<EventRequest> requestsForCustomer(const string& customerId) const;

    OpResult addHall(const string& name, int capacity, int64_t rentalPrice);
    OpResult updateHall(const string& id, const string& name, int capacity, int64_t rentalPrice,
                        HallStatus status);
    OpResult deleteHall(const string& id);
    OpResult addService(const string& code, const string& name, const string& unit, int64_t price,
                        const string& category);
    OpResult updateService(const string& id, const string& code, const string& name, const string& unit,
                           int64_t price, const string& category, bool active);
    OpResult deleteService(const string& id);
    // Quan ly nap bo sung danh muc mau dac ta (khong xoa du lieu hien co).
    OpResult importCatalog();
    const Hall* findHall(const string& id) const;
    const ServiceItem* findService(const string& id) const;
    const ServiceItem* findServiceByCode(const string& code) const;
    vector<string> serviceCategories() const;
    vector<Hall> findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                    const string& ignoreBookingId) const;
    bool hallAvailable(const string& hallId, DateTime start, DateTime end, const string& ignoreBookingId,
                       string& reason) const;
    // Ham nghiep vu throw; ham hallAvailable doi loi thanh thong bao cho Qt.
    void validateBookingSlot(const string& hallId, DateTime start, DateTime end,
                             const string& ignoreBookingId = "") const;
    int maxHallCapacity() const;

    // Dac ta 3.1: nut Tao Booking -> copy thong tin, request DaChot.
    OpResult createBooking(const string& requestId, const string& hallId, DateTime start, DateTime end,
                           int guestCount, int discountPercent, int64_t discountAmount);
    OpResult setBookingStatus(const string& bookingId, BookingStatus status);
    OpResult cancelBooking(const string& bookingId, const string& reason);
    OpResult deleteBooking(const string& bookingId);
    OpResult updateBookingDiscount(const string& bookingId, int discountPercent, int64_t discountAmount);
    OpResult approveDiscount(const string& bookingId);
    const Booking* findBooking(const string& id) const;
    vector<Booking> bookingsForCustomer(const string& customerId) const;
    // Quet het han giu cho (dac ta 3.3): tra ve so booking bi huy.
    int sweepExpiredHolds(DateTime now);
    // Lam moi trang thai su kien theo thoi gian (dac ta D).
    int refreshEventStatuses(DateTime now);

    // Form 3: dien khoan da tu tao (amount 0) hoac tao moi, danh dau DaThanhToan.
    OpResult recordPayment(const string& bookingId, PaymentType type, int64_t amount, PaymentMethod method,
                           DateTime paidAt, const string& note);
    OpResult deletePayment(const string& id);
    int64_t paidAmount(const string& bookingId) const;
    vector<Payment> paymentsOf(const string& bookingId) const;
    const Payment* findPayment(const string& id) const;
    string paymentReceipt(const string& paymentId) const;

    const Event* findEvent(const string& id) const;
    const Event* findEventByBooking(const string& bookingId) const;
    vector<shared_ptr<Event>> allEvents() const;
    vector<EventRequest> guestRequests() const;

    vector<string> userIds() const;
    vector<string> customerIds() const;

private:
    TextStorage& storage_;
    string currentUserId_;
    vector<string> guestRequestIds_;

    Database& mut() { return storage_.db(); }
    const Database& data() const { return storage_.db(); }

    template <typename T>
    T* findById(vector<T>& items, const string& id);
    template <typename T>
    const T* findById(const vector<T>& items, const string& id) const;

    OpResult persist(const string& successMessage, const string& id);
    const User& authenticate(const string& username, const string& password) const;
    bool uniqueUsername(const string& username, const string& exceptId) const;
    bool uniquePhone(const string& phone, const string& exceptId) const;
    // Dung dich vu theo don vi: /ban -> so ban, /khach -> so khach, con lai 1.
    int resolveQuantity(const ServiceItem& svc, int guestCount, int qty) const;
    OpResult buildLines(int guestCount, const vector<ServiceLineInput>& in,
                        vector<RequestServiceLine>& out) const;
    OpResult applyBookingFromRequest(const string& requestId, const string& hallId, DateTime start,
                                     DateTime end, int guestCount, int discountPercent,
                                     int64_t discountAmount, string& bookingId);
};

#include "storage/catalog_seed.cpp"
#include "utils/sha256.cpp"

#include <algorithm>

bool roleAllows(Role role, Action action) {
    if (action == Action::SubmitRequest) return role != Role::Operation;
    if (role == Role::Manager) return true;
    switch (action) {
        case Action::SubmitRequest: return true;
        case Action::ManageUsers:
        case Action::ManageVenues:
            return false;
        case Action::ManageCustomers:
        case Action::ManageBookings:
        case Action::ManagePayments:
            return role == Role::Sales;
        case Action::ManageRequests:
            // Khach hang tao/huy yeu cau cua chinh minh (dac ta A).
            return role == Role::Sales || role == Role::Customer;
        case Action::LookupHalls:
            return role == Role::Sales || role == Role::Customer;
        case Action::ManageEvents:
            return role == Role::Operation;
    }
    return false;
}

int64_t discountLimitFor(int64_t total) { return total * kDiscountApprovalPercent / 100; }

PalaceService::PalaceService(TextStorage& storage) : storage_(storage) {}

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
    if (!u) return action == Action::SubmitRequest || action == Action::LookupHalls;
    if (!u->active()) return false;
    return roleAllows(u->role(), action);
}

OpResult PalaceService::require(Action action) const {
    const User* u = currentUser();
    if (!u) return can(action) ? OpResult::success() : OpResult::fail("Chưa đăng nhập.");
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

const User& PalaceService::authenticate(const string& username, const string& password) const {
    for (const auto& user : data().users) {
        if (toLower(user.username()) != toLower(trim(username))) continue;
        if (!user.active()) throw AuthenticationException("Tài khoản đã bị khóa.");
        if (!verifyPassword(password, user.salt(), user.passwordHash()))
            throw AuthenticationException("Sai mật khẩu.");
        return user;
    }
    throw AuthenticationException("Không tìm thấy tài khoản.");
}

OpResult PalaceService::login(const string& username, const string& password) {
    try {
        const User& user = authenticate(username, password);
        currentUserId_ = user.id();
        sweepExpiredHolds(DateTime::now());
        refreshEventStatuses(DateTime::now());
        try {
            storage_.save();
        } catch (...) {
        }
        return OpResult::success("Xin chào " + user.fullName(), user.id());
    }
    catch (const AuthenticationException& ex) {
        return OpResult::fail(ex.what());
    }
}

void PalaceService::logout() { currentUserId_.clear(); guestRequestIds_.clear(); }

OpResult PalaceService::registerUser(const string& username, const string& password,
                                     const string& confirmPassword, const string& fullName,
                                     const string& phone, const string& email, const string& address) {
    if (trim(username).empty()) return OpResult::fail("Tên đăng nhập không được để trống.");
    if (password.size() < 4) return OpResult::fail("Mật khẩu tối thiểu 4 ký tự.");
    if (password != confirmPassword) return OpResult::fail("Mật khẩu nhập lại không khớp.");
    if (!uniqueUsername(trim(username), "")) return OpResult::fail("Tên đăng nhập đã tồn tại.");
    if (trim(fullName).empty()) return OpResult::fail("Họ tên là bắt buộc.");
    if (!looksLikePhone(trim(phone))) return OpResult::fail("Số điện thoại không hợp lệ (9-12 chữ số).");
    if (!uniquePhone(trim(phone), "")) return OpResult::fail("Số điện thoại đã được dùng cho hồ sơ khác.");
    if (trim(email).empty() || !looksLikeEmail(trim(email)))
        return OpResult::fail("Email là bắt buộc và phải đúng định dạng.");
    // Tu dong tao ho so khach hang lien ket tai khoan.
    string cusId = nextId("CUS-", customerIds());
    mut().customers.push_back(Customer(cusId, trim(fullName), trim(phone), trim(email), trim(address),
                                       "Tự đăng ký tài khoản", DateTime::now()));
    string id = nextId("USR-", userIds());
    string salt = randomSaltHex();
    User u(id, trim(username), salt, hashPassword(password, salt), trim(fullName), Role::Customer, true);
    u.setDemoPassword(password);
    u.setCustomerId(cusId);
    mut().users.push_back(move(u));
    return persist("Đăng ký thành công. Hồ sơ khách hàng " + cusId + " đã được tạo, bạn có thể đăng nhập.",
                   id);
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
    string id = nextId("USR-", userIds());
    string salt = randomSaltHex();
    User u(id, trim(username), salt, hashPassword(password, salt), trim(fullName), role, true);
    u.setDemoPassword(password);
    mut().users.push_back(move(u));
    return persist("Đã tạo tài khoản " + id, id);
}

static bool isLastActiveManager(const Database& data, const string& exceptId) {
    int n = 0;
    for (const auto& x : data.users) {
        if (x.id() != exceptId && x.active() && x.role() == Role::Manager) ++n;
    }
    return n == 0;
}

OpResult PalaceService::updateUserRole(const string& userId, Role role) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    auto* u = findById(mut().users, userId);
    if (!u) return OpResult::fail("Không tìm thấy tài khoản.");
    if (u->role() == Role::Manager && role != Role::Manager && u->active() &&
        isLastActiveManager(data(), userId))
        return OpResult::fail("Không thể hạ quyền quản lý cuối cùng.");
    u->setRole(role);
    return persist("Đã cập nhật vai trò.", userId);
}

OpResult PalaceService::deleteUser(const string& userId) {
    auto r = require(Action::ManageUsers);
    if (!r.ok) return r;
    auto* u = findById(mut().users, userId);
    if (!u) return OpResult::fail("Không tìm thấy tài khoản.");
    if (currentUserId_ == userId) return OpResult::fail("Không thể xóa tài khoản đang dùng.");
    if (u->role() == Role::Manager && u->active() && isLastActiveManager(data(), userId))
        return OpResult::fail("Không thể xóa quản lý cuối cùng.");
    auto& items = mut().users;
    items.erase(remove_if(items.begin(), items.end(), [&](const User& x) { return x.id() == userId; }),
                items.end());
    return persist("Đã xóa tài khoản.", userId);
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
const ServiceItem* PalaceService::findService(const string& id) const {
    return findById(data().services, id);
}
const ServiceItem* PalaceService::findServiceByCode(const string& code) const {
    for (const auto& s : data().services)
        if (s.code() == code) return &s;
    return nullptr;
}
const Booking* PalaceService::findBooking(const string& id) const { return findById(data().bookings, id); }
const Event* PalaceService::findEvent(const string& id) const {
    for (const auto& e : data().events) if (e->id() == id) return e.get();
    return nullptr;
}
const Payment* PalaceService::findPayment(const string& id) const { return findById(data().payments, id); }

const Event* PalaceService::findEventByBooking(const string& bookingId) const {
    for (const auto& e : data().events)
        if (e->bookingId() == bookingId) return e.get();
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

// ---- Yeu cau ----

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
                                   const string& notes, const vector<ServiceLineInput>& services, int customerAge) {
    auto r = require(Action::SubmitRequest);
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
    if (u && !findCustomer(cid)) return OpResult::fail("Khách hàng không tồn tại.");
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
    // Khach vang lai tu nhap ho so, khong duoc chon ID cua nguoi khac.
    if (!u) {
        if (!customerId.empty()) return OpResult::fail("Khách vãng lai không được chọn hồ sơ có sẵn.");
        if (customerAge < 1 || customerAge > 120) return OpResult::fail("Tuổi phải từ 1 đến 120.");
        // Chi tai su dung ho so trung SDT va ten, khong sua ho so da co.
        for (const auto& c : data().customers) {
            if (c.phone() != cphone) continue;
            if (toLower(c.name()) != toLower(cname))
                return OpResult::fail("Số điện thoại đã được dùng cho tên khác. Vui lòng liên hệ nhân viên.");
            cid = c.id();
        }
        if (cid.empty()) {
            cid = nextId("CUS-", customerIds());
            Customer c(cid, cname, cphone, "", "", "Khách vãng lai", DateTime::now());
            c.setAge(customerAge);
            mut().customers.push_back(c);
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
    if (!u) guestRequestIds_.push_back(id);
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

// ---- Sanh & dich vu ----

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

OpResult PalaceService::importCatalog() {
    auto r = require(Action::ManageVenues);
    if (!r.ok) return r;
    size_t hallsBefore = mut().halls.size(), svcBefore = mut().services.size();
    seedCatalog(mut());
    size_t addedHalls = mut().halls.size() - hallsBefore;
    size_t addedSvc = mut().services.size() - svcBefore;
    return persist("Đã nạp danh mục mẫu: +" + to_string(addedHalls) + " sảnh, +" +
                       to_string(addedSvc) + " dịch vụ (giữ nguyên dữ liệu cũ).",
                   "");
}

vector<string> PalaceService::serviceCategories() const {    vector<string> cats;
    for (const auto& s : data().services) {
        if (find(cats.begin(), cats.end(), s.category()) == cats.end()) cats.push_back(s.category());
    }
    return cats;
}

bool PalaceService::hallAvailable(const string& hallId, DateTime start, DateTime end,
                                  const string& ignoreBookingId, string& reason) const {
    try {
        validateBookingSlot(hallId, start, end, ignoreBookingId);
        reason.clear();
        return true;
    } catch (const AppException& ex) {
        reason = ex.what();
        return false;
    }
}

void PalaceService::validateBookingSlot(const string& hallId, DateTime start, DateTime end,
                                       const string& ignoreBookingId) const {
    const Hall* hall = findHall(hallId);
    if (!hall) throw NotFoundException("Sảnh không tồn tại.");
    if (hall->status() != HallStatus::KhaDung) {
        throw ValidationException("Sảnh '" + hall->name() + "' hiện ở trạng thái " + toString(hall->status()) + ".");
    }
    if (!start.valid() || !end.valid() || !(start < end))
        throw ValidationException("Thời gian bắt đầu phải hợp lệ và sớm hơn kết thúc.");
    for (const auto& b : data().bookings) {
        if (!b.occupiesHall()) continue;
        if (b.id() == ignoreBookingId) continue;
        if (b.hallId() != hallId) continue;
        if (timeOverlaps(start, end, b.start(), b.end())) {
            throw BookingConflictException("Sảnh bị trùng lịch với booking " + b.id() + " (" +
                b.start().toString() + " - " + b.end().toString() + ").");
        }
    }
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

// ---- Booking ----

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
    vector<string> pids;
    for (const auto& x : data().payments) pids.push_back(x.id());
    string pid = nextId("PAY-", pids);
    mut().payments.push_back(Payment(pid, id, PaymentType::DatCoc1, 0, now, PaymentMethod::ChuyenKhoan,
                                     PaymentStatus::ChuaThanhToan, "Tiền đặt cọc giữ chỗ",
                                     actor ? actor->username() : ""));

    // Su kien tu dong tao tu Booking (dac ta D).
    vector<string> eids;
    for (const auto& x : data().events) eids.push_back(x->id());
    string eid = nextId("EVT-", eids);
    mut().events.push_back(makeEvent(req->type(), eid, id, req->customerId(), hallId, start, end, EventStatus::SapDienRa,
                                 req->notes()));
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
        if (ev->bookingId() == bookingId) ev->setStatus(EventStatus::Huy);
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
                            [&](const shared_ptr<Event>& e) { return e->bookingId() == bookingId; }),
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
                if (ev->bookingId() == b.id()) ev->setStatus(EventStatus::Huy);
            }
            ++n;
        }
    }
    return n;
}

int PalaceService::refreshEventStatuses(DateTime now) {
    int n = 0;
    for (auto& e : mut().events) {
        if (e->status() == EventStatus::Huy) continue;
        EventStatus want = eventTimeStatus(e->start(), e->end(), now);
        if (want != e->status()) {
            e->setStatus(want);
            ++n;
        }
    }
    return n;
}

// ---- Thanh toan ----

OpResult PalaceService::recordPayment(const string& bookingId, PaymentType type, int64_t amount,
                                      PaymentMethod method, DateTime paidAt, const string& note) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    const Booking* b = findBooking(bookingId);
    if (!b) return OpResult::fail("Booking không tồn tại.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Không thu tiền cho booking đã hủy.");
    if (amount <= 0) return OpResult::fail("Nhân viên phải nhập giá trị khoản thanh toán (> 0).");
    if (trim(note).empty()) return OpResult::fail("Ghi chú thanh toán là bắt buộc.");
    if (!paidAt.valid()) return OpResult::fail("Ngày thanh toán không hợp lệ.");
    if (DateTime::now() < paidAt)
        return OpResult::fail("Ngày thanh toán không được trong tương lai.");
    const User* actor = currentUser();
    auto* slot = (Payment*)nullptr;
    for (auto& p : mut().payments) {
        if (p.bookingId() == bookingId && p.type() == type && p.status() == PaymentStatus::ChuaThanhToan) {
            slot = &p;
            break;
        }
    }
    string id;
    if (slot) {
        slot->setAmount(amount);
        slot->setMethod(method);
        slot->setPaidAt(paidAt);
        slot->setNote(trim(note));
        slot->setStatus(PaymentStatus::DaThanhToan);
        id = slot->id();
    } else {
        vector<string> ids;
        for (const auto& x : data().payments) ids.push_back(x.id());
        id = nextId("PAY-", ids);
        mut().payments.push_back(Payment(id, bookingId, type, amount, paidAt, method,
                                         PaymentStatus::DaThanhToan, trim(note),
                                         actor ? actor->username() : ""));
    }
    // Booking chuyen Da coc khi tien coc hoan thanh (dac ta B.2 + 3.3).
    if (auto* bb = findById(mut().bookings, bookingId)) {
        if ((type == PaymentType::DatCoc1 || type == PaymentType::DatCoc2) &&
            bb->status() == BookingStatus::ChoCoc) {
            bb->setStatus(BookingStatus::DaCoc);
        }
        if (type == PaymentType::TatToan && bb->status() == BookingStatus::DaCoc) {
            bb->setStatus(BookingStatus::HoanTat);
        }
    }
    return persist("Đã ghi nhận " + toString(type) + " " + formatMoney(amount) + " cho booking " + bookingId +
                       ".",
                   id);
}

OpResult PalaceService::deletePayment(const string& id) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    auto* p = findById(mut().payments, id);
    if (!p) return OpResult::fail("Không tìm thấy khoản thanh toán.");
    if (p->status() == PaymentStatus::DaThanhToan)
        return OpResult::fail("Khoản đã thanh toán không được xóa.");
    auto& items = mut().payments;
    items.erase(remove_if(items.begin(), items.end(), [&](const Payment& x) { return x.id() == id; }),
                items.end());
    return persist("Đã xóa khoản thanh toán.", id);
}

int64_t PalaceService::paidAmount(const string& bookingId) const {
    int64_t sum = 0;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId && p.status() == PaymentStatus::DaThanhToan && p.amount() > 0)
            sum += p.amount();
    return sum;
}

vector<Payment> PalaceService::paymentsOf(const string& bookingId) const {
    vector<Payment> out;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId) out.push_back(p);
    return out;
}

string PalaceService::paymentReceipt(const string& paymentId) const {
    const Payment* p = findPayment(paymentId);
    if (!p) return "Không tìm thấy phiếu thu.";
    const Booking* b = findBooking(p->bookingId());
    const Customer* c = b ? findCustomer(b->customerId()) : nullptr;
    const Hall* h = b ? findHall(b->hallId()) : nullptr;
    ostringstream ss;
    ss << "========== PHIẾU THU ==========\n";
    ss << "Số phiếu: " << p->id() << "\n";
    ss << "Booking: " << p->bookingId() << "\n";
    if (c) ss << "Khách hàng: " << c->name() << " - " << c->phone() << "\n";
    if (h) ss << "Sảnh: " << h->name() << "\n";
    if (b) ss << "Tổng booking: " << formatMoney(b->finalTotal()) << "\n";
    ss << "Khoản thu: " << toString(p->type()) << "\n";
    ss << "Số tiền: " << formatMoney(p->amount()) << "\n";
    ss << "Hình thức: " << toString(p->method()) << "\n";
    ss << "Ngày thu: " << p->paidAt().toString() << "\n";
    ss << "Trạng thái: " << toString(p->status()) << "\n";
    ss << "Ghi chú: " << p->note() << "\n";
    ss << "Người thu: " << p->recordedBy() << "\n";
    ss << "==============================";
    return ss.str();
}

vector<shared_ptr<Event>> PalaceService::allEvents() const { return data().events; }

vector<EventRequest> PalaceService::guestRequests() const {
    vector<EventRequest> out;
    for (const auto& id : guestRequestIds_)
        if (const auto* r = findRequest(id)) out.push_back(*r);
    return out;
}

#endif
