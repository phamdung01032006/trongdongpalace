#include "models/Hall.hpp"

using namespace std;

namespace tdp {

Hall::Hall(string id, string centerId, string name, int capacity, bool active)
    : id_(move(id)), centerId_(move(centerId)), name_(move(name)), capacity_(capacity), active_(active) {}

nlohmann::json Hall::toJson() const {
    return {{"id", id_}, {"centerId", centerId_}, {"name", name_}, {"capacity", capacity_}, {"active", active_}};
}

Hall Hall::fromJson(const nlohmann::json& j) {
    return Hall(j.at("id").get<string>(), j.at("centerId").get<string>(), j.at("name").get<string>(),
                j.value("capacity", 0), j.value("active", true));
}

}  // namespace tdp
