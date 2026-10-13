#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class User {
public:
    User() = default;
    User(string id, string username, string salt, string passwordHash,
         string fullName, Role role, bool active);

    const string& id() const { return id_; }
    const string& username() const { return username_; }
    const string& salt() const { return salt_; }
    const string& passwordHash() const { return passwordHash_; }
    const string& fullName() const { return fullName_; }
    Role role() const { return role_; }
    bool active() const { return active_; }
    // Tai khoan Customer lien ket 1 khach hang de tu dien form (dac ta Form 1).
    const string& customerId() const { return customerId_; }
    // Mật khẩu dạng rõ chỉ phục vụ demo; đăng nhập vẫn kiểm tra bằng salt + hash.
    const string& demoPassword() const { return demoPassword_; }

    void setFullName(const string& v) { fullName_ = v; }
    void setRole(Role v) { role_ = v; }
    void setActive(bool v) { active_ = v; }
    void setCustomerId(const string& v) { customerId_ = v; }
    void setDemoPassword(const string& v) { demoPassword_ = v; }
    void setPassword(const string& salt, const string& hash) {
        salt_ = salt;
        passwordHash_ = hash;
    }

    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& j);

private:
    string id_;
    string username_;
    string salt_;
    string passwordHash_;
    string fullName_;
    string customerId_;
    string demoPassword_;  // ghi rõ trong JSON để thuận tiện demo
    Role role_ = Role::Customer;
    bool active_ = true;
};

}  // namespace tdp
