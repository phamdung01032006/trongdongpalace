#include "models/Hall.hpp"

namespace tdp {

Hall::Hall(std::string id, std::string centerId, std::string name, int capacity, bool active)
    : id_(std::move(id)), centerId_(std::move(centerId)), name_(std::move(name)), capacity_(capacity), active_(active) {}

nlohmann::json Hall::toJson() const {
    return {{"id", id_}, {"centerId", centerId_}, {"name", name_}, {"capacity", capacity_}, {"active", active_}};
}

Hall Hall::fromJson(const nlohmann::json& j) {
    return Hall(j.at("id").get<std::string>(), j.at("centerId").get<std::string>(), j.at("name").get<std::string>(),
                j.value("capacity", 0), j.value("active", true));
}

}  // namespace tdp
