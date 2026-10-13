#include "models/Hall.hpp"

using namespace std;

namespace tdp {

Hall::Hall(string id, string name, int capacity, int64_t rentalPrice, HallStatus status)
    : id_(move(id)), name_(move(name)), capacity_(capacity), rentalPrice_(rentalPrice), status_(status) {}

nlohmann::json Hall::toJson() const {
    return {{"id", id_},
            {"name", name_},
            {"capacity", capacity_},
            {"rentalPrice", rentalPrice_},
            {"status", toString(status_)},
            {"active", status_ == HallStatus::KhaDung}};
}

Hall Hall::fromJson(const nlohmann::json& j) {
    HallStatus status = HallStatus::KhaDung;
    if (j.contains("status")) {
        status = hallStatusFromString(j.value("status", "Khả dụng"));
    } else if (j.contains("active")) {
        status = j.value("active", true) ? HallStatus::KhaDung : HallStatus::KhongKhaDung;
    }
    return Hall(j.at("id").get<string>(), j.at("name").get<string>(), j.value("capacity", 0),
                j.value("rentalPrice", int64_t(0)), status);
}

}  // namespace tdp
