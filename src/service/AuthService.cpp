#include "AuthService.h"
#include "Exceptions.h"

using namespace std;

AuthService::AuthService(UserRepository& userRepository) : userRepository(userRepository) {}

void AuthService::seedDefaultUsers() {
    const vector<User> defaultUsers = {
        User("U001", "admin", "admin123", "Administrator", "ADMIN"),
        User("U002", "sales", "sales123", "Sales Staff", "SALES"),
        User("U003", "accountant", "accountant123", "Accountant", "ACCOUNTANT"),
        User("U004", "coordinator", "coord123", "Coordinator", "COORDINATOR"),
        User("U005", "manager", "manager123", "Management", "MANAGEMENT")
    };

    for (const User& user : defaultUsers) {
        try {
            userRepository.findByUsername(user.getUsername());
        } catch (const NotFoundException&) {
            userRepository.add(user);
        }
    }
}

User AuthService::login(const string& username, const string& password) const {
    if (username.empty() || password.empty()) {
        throw AuthenticationException("Username and password cannot be empty.");
    }
    try {
        const User user = userRepository.findByUsername(username);
        if (user.getPassword() != password) {
            throw AuthenticationException("Invalid username or password.");
        }
        return user;
    } catch (const NotFoundException&) {
        throw AuthenticationException("Invalid username or password.");
    }
}

bool AuthService::hasPermission(const User& user, const string& module, const string& action) const {
    if (user.getRole() == "ADMIN") return true;

    const bool readAction = action == "READ";
    if (user.getRole() == "MANAGEMENT") return readAction || module == "REPORT";
    if (user.getRole() == "SALES") {
        return module == "CUSTOMER" || module == "EVENT" || module == "BOOKING" ||
               module == "QUOTATION" || module == "CONTRACT" ||
               ((module == "CENTER" || module == "HALL") && readAction) ||
               (module == "REPORT" && readAction);
    }
    if (user.getRole() == "ACCOUNTANT") {
        return module == "PAYMENT" || readAction;
    }
    if (user.getRole() == "COORDINATOR") {
        return module == "CENTER" || module == "HALL" || module == "EVENT" ||
               module == "OPERATIONS" || readAction;
    }
    return false;
}

void AuthService::requirePermission(const User& user, const string& module, const string& action) const {
    if (!hasPermission(user, module, action)) {
        throw AuthenticationException("Access denied.");
    }
}
