#include "models/ServiceItem.hpp"

using namespace std;

namespace tdp {

ServiceItem::ServiceItem(string id, string name, string unit, int64_t unitPrice, string category,
                         bool active)
    : id_(move(id)),
      name_(move(name)),
      unit_(move(unit)),
      category_(move(category)),
      unitPrice_(unitPrice),
      active_(active) {}

nlohmann::json ServiceItem::toJson() const {
    return {{"id", id_}, {"name", name_}, {"unit", unit_}, {"unitPrice", unitPrice_}, {"category", category_},
            {"active", active_}};
}

ServiceItem ServiceItem::fromJson(const nlohmann::json& j) {
    return ServiceItem(j.at("id").get<string>(), j.at("name").get<string>(), j.value("unit", "lần"),
                       j.value("unitPrice", 0), j.value("category", "Khac"), j.value("active", true));
}

}  // namespace tdp
