#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class User {
public:
    User() = default;
    User(std::string id, std::string username, std::string salt, std::string passwordHash,
         std::string fullName, Role role, bool active);

    const std::string& id() const { return id_; }
    const std::string& username() const { return username_; }
    const std::string& salt() const { return salt_; }
    const std::string& passwordHash() const { return passwordHash_; }
    const std::string& fullName() const { return fullName_; }
    Role role() const { return role_; }
    bool active() const { return active_; }
    // Mật khẩu dạng rõ chỉ phục vụ demo; đăng nhập vẫn kiểm tra bằng salt + hash.
    const std::string& demoPassword() const { return demoPassword_; }

    void setFullName(const std::string& v) { fullName_ = v; }
    void setRole(Role v) { role_ = v; }
    void setActive(bool v) { active_ = v; }
    void setDemoPassword(const std::string& v) { demoPassword_ = v; }
    void setPassword(const std::string& salt, const std::string& hash) {
        salt_ = salt;
        passwordHash_ = hash;
    }

    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& j);

private:
    std::string id_;
    std::string username_;
    std::string salt_;
    std::string passwordHash_;
    std::string fullName_;
    std::string demoPassword_;  // ghi rõ trong JSON để thuận tiện demo
    Role role_ = Role::Sales;
    bool active_ = true;
};

}  // namespace tdp
