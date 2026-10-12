#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class ServiceItem {
public:
    ServiceItem() = default;
    ServiceItem(std::string id, std::string name, std::string unit, int64_t unitPrice, std::string category,
                bool active);
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& unit() const { return unit_; }
    int64_t unitPrice() const { return unitPrice_; }
    const std::string& category() const { return category_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setUnit(std::string v) { unit_ = std::move(v); }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    void setCategory(std::string v) { category_ = std::move(v); }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static ServiceItem fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, unit_, category_;
    int64_t unitPrice_ = 0;
    bool active_ = true;
};

}  // namespace tdp
