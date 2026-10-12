#include "models/Center.hpp"

namespace tdp {

Center::Center(std::string id, std::string name, std::string address, bool active)
    : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)), active_(active) {}

nlohmann::json Center::toJson() const {
    return {{"id", id_}, {"name", name_}, {"address", address_}, {"active", active_}};
}

Center Center::fromJson(const nlohmann::json& j) {
    return Center(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.value("address", ""),
                  j.value("active", true));
}

}  // namespace tdp
