#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Center {
public:
    Center() = default;
    Center(std::string id, std::string name, std::string address, bool active);
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& address() const { return address_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setAddress(std::string v) { address_ = std::move(v); }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Center fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, address_;
    bool active_ = true;
};

}  // namespace tdp
