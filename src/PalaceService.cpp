#include "PalaceService.h"

#include "Utils.h"

#include <algorithm>
#include <exception>

using namespace std;

namespace {

// Bang phan quyen. Quan ly duoc lam moi thu, cac vai tro khac bi gioi han.
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
            return role == Role::Sales || role == Role::Customer;
        case Action::LookupHalls:
            return role == Role::Sales || role == Role::Customer;
        case Action::ManageEvents:
            return role == Role::Operation;
    }
    return false;
}

bool isLastActiveManager(const Database& data, const string& exceptId) {
    int count = 0;
    for (const User& user : data.get_users()) {
        if (user.get_id() != exceptId && user.is_active() && user.get_role() == Role::Manager) ++count;
    }
    return count == 0;
}

}  // namespace

PalaceService::PalaceService(Store& store) : store_(store) {}

// ==================== DANG NHAP / PHAN QUYEN ====================

const User* PalaceService::currentUser() const {
    if (currentUserId_.empty()) return nullptr;
    return findById(get_db().get_users(), currentUserId_);
}

bool PalaceService::can(Action action) const {
    const User* user = currentUser();
    if (user == nullptr || !user->is_active()) return false;
    return roleAllows(user->get_role(), action);
}

void PalaceService::require(Action action) const {
    const User* user = currentUser();
    if (user == nullptr) throw BusinessException("Chưa đăng nhập.");
    if (!user->is_active()) throw BusinessException("Tài khoản đã bị khóa.");
    if (!can(action)) throw BusinessException("Bạn không có quyền thực hiện thao tác này.");
}

OpResult PalaceService::login(const string& username, const string& password) {
    for (const User& user : get_db().get_users()) {
        if (toLower(user.get_username()) != toLower(trim(username))) continue;
        if (!user.is_active()) throw BusinessException("Tài khoản đã bị khóa.");
        if (user.get_password() != password) throw BusinessException("Sai mật khẩu.");

        currentUserId_ = user.get_id();
        refreshSystem();  // Huy booking qua han + cap nhat su kien.
        return OpResult::success("Xin chào " + user.get_fullName(), user.get_id());
    }
    throw BusinessException("Không tìm thấy tài khoản.");
}

void PalaceService::logout() { currentUserId_.clear(); }

OpResult PalaceService::registerCustomer(const string& username, const string& password,
                                         const string& confirmPassword, const string& fullName,
                                         const string& phone, const string& email,
                                         const string& address) {
    if (trim(username).empty()) throw BusinessException("Tên đăng nhập không được để trống.");
    if (password.size() < 4) throw BusinessException("Mật khẩu tối thiểu 4 ký tự.");
    if (password != confirmPassword) throw BusinessException("Mật khẩu nhập lại không khớp.");
    if (!uniqueUsername(trim(username), "")) throw BusinessException("Tên đăng nhập đã tồn tại.");
    if (trim(fullName).empty()) throw BusinessException("Họ tên là bắt buộc.");
    if (!looksLikePhone(trim(phone))) throw BusinessException("Số điện thoại không hợp lệ (9-12 chữ số).");
    if (!uniquePhone(trim(phone), "")) throw BusinessException("Số điện thoại đã được dùng.");
    if (!looksLikeEmail(trim(email))) throw BusinessException("Email không đúng định dạng.");

    string customerId = nextId("CUS-", customerIds());
    get_db().get_customers().push_back(Customer(customerId, trim(fullName), trim(phone), trim(email),
                                       trim(address), "Tự đăng ký tài khoản"));

    string userId = nextId("USR-", userIds());
    get_db().get_users().push_back(User(userId, trim(username), password, trim(fullName), trim(phone),
                              trim(email), trim(address), Role::Customer, customerId, true));

    return persist("Đăng ký thành công. Hồ sơ khách hàng " + customerId + " đã được tạo.", userId);
}

// ==================== QUAN LY TAI KHOAN ====================

OpResult PalaceService::createUser(const string& username, const string& password,
                                   const string& fullName, Role role) {
    require(Action::ManageUsers);

    if (role == Role::Customer)
        throw BusinessException("Tài khoản khách hàng phải do khách tự đăng ký.");
    if (trim(username).empty() || trim(fullName).empty())
        throw BusinessException("Tên đăng nhập và họ tên là bắt buộc.");
    if (password.size() < 4) throw BusinessException("Mật khẩu tối thiểu 4 ký tự.");
    if (!uniqueUsername(trim(username), "")) throw BusinessException("Tên đăng nhập đã tồn tại.");

    string id = nextId("USR-", userIds());
    get_db().get_users().push_back(User(id, trim(username), password, trim(fullName), "", "", "", role,
                              "", true));
    return persist("Đã tạo tài khoản " + id, id);
}

OpResult PalaceService::updateUserRole(const string& userId, Role role) {
    require(Action::ManageUsers);

    User* target = nullptr;
    for (User& user : get_db().get_users()) {
        if (user.get_id() == userId) {
            target = &user;
            break;
        }
    }
    if (target == nullptr) throw BusinessException("Không tìm thấy tài khoản.");

    if (target->get_role() == Role::Manager && role != Role::Manager && target->is_active() &&
        isLastActiveManager(get_db(), userId)) {
        throw BusinessException("Không thể hạ quyền quản lý cuối cùng.");
    }
    if (role == Role::Customer && target->get_customerId().empty()) {
        throw BusinessException("Tài khoản này chưa gắn hồ sơ khách hàng, không thể đổi thành Khách hàng.");
    }

    target->set_role(role);
    return persist("Đã cập nhật vai trò.", userId);
}

OpResult PalaceService::deleteUser(const string& userId) {
    require(Action::ManageUsers);

    const User* target = nullptr;
    for (const User& user : get_db().get_users()) {
        if (user.get_id() == userId) {
            target = &user;
            break;
        }
    }
    if (target == nullptr) throw BusinessException("Không tìm thấy tài khoản.");
    if (currentUserId_ == userId) throw BusinessException("Không thể xóa tài khoản đang dùng.");
    if (target->get_role() == Role::Manager && target->is_active() && isLastActiveManager(get_db(), userId))
        throw BusinessException("Không thể xóa quản lý cuối cùng.");

    vector<User>& users = get_db().get_users();
    users.erase(remove_if(users.begin(), users.end(),
                          [&](const User& user) { return user.get_id() == userId; }),
                users.end());
    return persist("Đã xóa tài khoản.", userId);
}

OpResult PalaceService::setUserActive(const string& userId, bool active) {
    require(Action::ManageUsers);

    User* target = nullptr;
    for (User& user : get_db().get_users()) {
        if (user.get_id() == userId) {
            target = &user;
            break;
        }
    }
    if (target == nullptr) throw BusinessException("Không tìm thấy tài khoản.");
    if (currentUserId_ == userId && !active)
        throw BusinessException("Không thể tự khóa tài khoản đang dùng.");
    if (!active && target->get_role() == Role::Manager && target->is_active() &&
        isLastActiveManager(get_db(), userId)) {
        throw BusinessException("Không thể khóa quản lý cuối cùng.");
    }

    target->set_active(active);
    return persist(active ? "Đã mở khóa tài khoản." : "Đã khóa tài khoản.", userId);
}

OpResult PalaceService::changePassword(const string& userId, const string& newPassword) {
    require(Action::ManageUsers);
    if (newPassword.size() < 4) throw BusinessException("Mật khẩu tối thiểu 4 ký tự.");

    for (User& user : get_db().get_users()) {
        if (user.get_id() == userId) {
            user.set_password(newPassword);
            return persist("Đã đổi mật khẩu.", userId);
        }
    }
    throw BusinessException("Không tìm thấy tài khoản.");
}

// ==================== KHACH HANG ====================

OpResult PalaceService::addCustomer(const string& name, const string& phone, const string& email,
                                    const string& address, const string& notes) {
    require(Action::ManageCustomers);

    if (trim(name).empty()) throw BusinessException("Tên khách hàng là bắt buộc.");
    if (!looksLikePhone(trim(phone))) throw BusinessException("Số điện thoại không hợp lệ.");
    if (!looksLikeEmail(trim(email))) throw BusinessException("Email không đúng định dạng.");
    if (!uniquePhone(trim(phone), "")) throw BusinessException("Số điện thoại đã tồn tại.");

    string id = nextId("CUS-", customerIds());
    get_db().get_customers().push_back(Customer(id, trim(name), trim(phone), trim(email), trim(address),
                                       trim(notes)));
    return persist("Đã thêm khách hàng " + id, id);
}

OpResult PalaceService::updateCustomer(const string& id, const string& name, const string& phone,
                                       const string& email, const string& address,
                                       const string& notes) {
    require(Action::ManageCustomers);

    for (Customer& customer : get_db().get_customers()) {
        if (customer.get_id() != id) continue;
        if (trim(name).empty()) throw BusinessException("Tên khách hàng là bắt buộc.");
        if (!looksLikePhone(trim(phone))) throw BusinessException("Số điện thoại không hợp lệ.");
        if (!looksLikeEmail(trim(email))) throw BusinessException("Email không đúng định dạng.");
        if (!uniquePhone(trim(phone), id)) throw BusinessException("Số điện thoại đã tồn tại.");

        customer.set_fullName(trim(name));
        customer.set_phone(trim(phone));
        customer.set_email(trim(email));
        customer.set_address(trim(address));
        customer.set_notes(trim(notes));
        return persist("Đã cập nhật khách hàng.", id);
    }
    throw BusinessException("Không tìm thấy khách hàng.");
}

OpResult PalaceService::deleteCustomer(const string& id) {
    require(Action::ManageCustomers);

    const Customer* target = findCustomer(id);
    if (target == nullptr) throw BusinessException("Không tìm thấy khách hàng.");

    for (const EventRequest& request : get_db().get_requests())
        if (request.get_customerId() == id) throw BusinessException("Khách hàng đã có yêu cầu, không thể xóa.");
    for (const Booking& booking : get_db().get_bookings())
        if (booking.get_customerId() == id) throw BusinessException("Khách hàng đã có booking, không thể xóa.");
    for (const User& user : get_db().get_users())
        if (user.get_customerId() == id) throw BusinessException("Khách hàng đang gắn tài khoản, không thể xóa.");

    vector<Customer>& customers = get_db().get_customers();
    customers.erase(remove_if(customers.begin(), customers.end(),
                              [&](const Customer& customer) { return customer.get_id() == id; }),
                    customers.end());
    return persist("Đã xóa khách hàng.", id);
}

vector<Customer> PalaceService::searchCustomers(const string& keyword) const {
    vector<Customer> result;
    for (const Customer& customer : get_db().get_customers()) {
        if (keyword.empty() || containsIgnoreCase(customer.get_id(), keyword) ||
            containsIgnoreCase(customer.get_fullName(), keyword) ||
            containsIgnoreCase(customer.get_phone(), keyword)) {
            result.push_back(customer);
        }
    }
    return result;
}

const Customer* PalaceService::findCustomer(const string& id) const {
    return findById(get_db().get_customers(), id);
}

// ==================== HAM HO TRO ====================

OpResult PalaceService::persist(const string& successMessage, const string& id) {
    try {
        store_.save();
        return OpResult::success(successMessage, id);
    } catch (const exception& ex) {
        throw BusinessException(string("Lưu dữ liệu thất bại: ") + ex.what());
    }
}

bool PalaceService::uniqueUsername(const string& username, const string& exceptId) const {
    for (const User& user : get_db().get_users()) {
        if (user.get_id() != exceptId && toLower(user.get_username()) == toLower(username)) return false;
    }
    return true;
}

bool PalaceService::uniquePhone(const string& phone, const string& exceptId) const {
    for (const Customer& customer : get_db().get_customers()) {
        if (customer.get_id() != exceptId && customer.get_phone() == phone) return false;
    }
    return true;
}

vector<string> PalaceService::userIds() const {
    vector<string> ids;
    for (const User& user : get_db().get_users()) ids.push_back(user.get_id());
    return ids;
}

vector<string> PalaceService::customerIds() const {
    vector<string> ids;
    for (const Customer& customer : get_db().get_customers()) ids.push_back(customer.get_id());
    return ids;
}

vector<string> PalaceService::requestIds() const {
    vector<string> ids;
    for (const EventRequest& request : get_db().get_requests()) ids.push_back(request.get_id());
    return ids;
}

vector<string> PalaceService::bookingIds() const {
    vector<string> ids;
    for (const Booking& booking : get_db().get_bookings()) ids.push_back(booking.get_id());
    return ids;
}

vector<string> PalaceService::paymentIds() const {
    vector<string> ids;
    for (const Payment& payment : get_db().get_payments()) ids.push_back(payment.get_id());
    return ids;
}

vector<string> PalaceService::eventIds() const {
    vector<string> ids;
    for (const Event& event : get_db().get_events()) ids.push_back(event.get_id());
    return ids;
}

void PalaceService::refreshSystem() {
    DateTime now = DateTime::now();
    int expired = sweepExpiredHolds(now);
    int changed = refreshEventStatuses(now);

    if (expired > 0 || changed > 0) {
        try {
            store_.save();
        } catch (const exception&) {
            // Khong lam sap chuong trinh chi vi loi ghi file.
        }
    }
}
