#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class ServiceItem {
public:
    ServiceItem() = default;
    ServiceItem(string id, string code, string name, string unit, int64_t unitPrice, string category,
                bool active);
    const string& id() const { return id_; }
    const string& code() const { return code_; }
    const string& name() const { return name_; }
    const string& unit() const { return unit_; }
    int64_t unitPrice() const { return unitPrice_; }
    const string& category() const { return category_; }
    bool active() const { return active_; }
    void setName(string v) { name_ = move(v); }
    void setUnit(string v) { unit_ = move(v); }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    void setCategory(string v) { category_ = move(v); }
    void setActive(bool v) { active_ = v; }
    void setCode(string v) { code_ = move(v); }
    nlohmann::json toJson() const;
    static ServiceItem fromJson(const nlohmann::json& j);

private:
    string id_, code_, name_, unit_, category_;
    int64_t unitPrice_ = 0;
    bool active_ = true;
};

}  // namespace tdp
