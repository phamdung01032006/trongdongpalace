#include "models/Center.hpp"

using namespace std;

namespace tdp {

Center::Center(string id, string name, string address, bool active)
    : id_(move(id)), name_(move(name)), address_(move(address)), active_(active) {}

nlohmann::json Center::toJson() const {
    return {{"id", id_}, {"name", name_}, {"address", address_}, {"active", active_}};
}

Center Center::fromJson(const nlohmann::json& j) {
    return Center(j.at("id").get<string>(), j.at("name").get<string>(), j.value("address", ""),
                  j.value("active", true));
}

}  // namespace tdp
