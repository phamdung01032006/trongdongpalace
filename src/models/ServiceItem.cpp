#include "models/ServiceItem.hpp"

namespace tdp {

ServiceItem::ServiceItem(std::string id, std::string name, std::string unit, int64_t unitPrice, std::string category,
                         bool active)
    : id_(std::move(id)),
      name_(std::move(name)),
      unit_(std::move(unit)),
      category_(std::move(category)),
      unitPrice_(unitPrice),
      active_(active) {}

nlohmann::json ServiceItem::toJson() const {
    return {{"id", id_}, {"name", name_}, {"unit", unit_}, {"unitPrice", unitPrice_}, {"category", category_},
            {"active", active_}};
}

ServiceItem ServiceItem::fromJson(const nlohmann::json& j) {
    return ServiceItem(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.value("unit", "lần"),
                       j.value("unitPrice", 0), j.value("category", "Khac"), j.value("active", true));
}

}  // namespace tdp
