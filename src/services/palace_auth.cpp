#include "services/palace_service.hpp"

#include "utils/sha256.hpp"

#include <algorithm>

using namespace std;

namespace tdp {

bool roleAllows(Role role, Action action) {
    if (role == Role::Manager) return true;
    switch (action) {
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

PalaceService::PalaceService(JsonStorage& storage) : storage_(storage) {}

const User* PalaceService::currentUser() const {
    if (currentUserId_.empty()) return nullptr;
    return findById(data().users, currentUserId_);
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
        sweepExpiredHolds(DateTime::now());
        refreshEventStatuses(DateTime::now());
        try {
            storage_.save();
        } catch (...) {
        }
        return OpResult::success("Xin chào " + user.fullName(), user.id());
    }
    return OpResult::fail("Không tìm thấy tài khoản.");
}

void PalaceService::logout() { currentUserId_.clear(); }

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

const Event* PalaceService::findEvent(const string& id) const { return findById(data().events, id); }

const Payment* PalaceService::findPayment(const string& id) const { return findById(data().payments, id); }

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

}  // namespace tdp
