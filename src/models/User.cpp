#include "models/User.hpp"

namespace tdp {

User::User(std::string id, std::string username, std::string salt, std::string passwordHash, std::string fullName,
           Role role, bool active)
    : id_(std::move(id)),
      username_(std::move(username)),
      salt_(std::move(salt)),
      passwordHash_(std::move(passwordHash)),
      fullName_(std::move(fullName)),
      role_(role),
      active_(active) {}

nlohmann::json User::toJson() const {
    return {{"id", id_},
            {"username", username_},
            {"salt", salt_},
            {"passwordHash", passwordHash_},
            {"demoPassword", demoPassword_},
            {"fullName", fullName_},
            {"role", toString(role_)},
            {"active", active_}};
}

User User::fromJson(const nlohmann::json& j) {
    User u(j.at("id").get<std::string>(), j.at("username").get<std::string>(), j.at("salt").get<std::string>(),
           j.at("passwordHash").get<std::string>(), j.value("fullName", ""),
           roleFromString(j.value("role", "Sales")), j.value("active", true));
    u.setDemoPassword(j.value("demoPassword", ""));
    return u;
}

}  // namespace tdp
