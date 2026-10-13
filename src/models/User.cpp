#include "models/User.hpp"

using namespace std;

namespace tdp {

User::User(string id, string username, string salt, string passwordHash, string fullName,
           Role role, bool active)
    : id_(move(id)),
      username_(move(username)),
      salt_(move(salt)),
      passwordHash_(move(passwordHash)),
      fullName_(move(fullName)),
      role_(role),
      active_(active) {}

nlohmann::json User::toJson() const {
    return {{"id", id_},
            {"username", username_},
            {"salt", salt_},
            {"passwordHash", passwordHash_},
            {"demoPassword", demoPassword_},
            {"fullName", fullName_},
            {"customerId", customerId_},
            {"role", toString(role_)},
            {"active", active_}};
}

User User::fromJson(const nlohmann::json& j) {
    User u(j.at("id").get<string>(), j.at("username").get<string>(), j.at("salt").get<string>(),
           j.at("passwordHash").get<string>(), j.value("fullName", ""),
           roleFromString(j.value("role", "Khách hàng")), j.value("active", true));
    u.setDemoPassword(j.value("demoPassword", ""));
    u.setCustomerId(j.value("customerId", ""));
    return u;
}

}  // namespace tdp
