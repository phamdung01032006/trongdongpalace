#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Hall {
public:
    Hall() = default;
    Hall(std::string id, std::string centerId, std::string name, int capacity, bool active);
    const std::string& id() const { return id_; }
    const std::string& centerId() const { return centerId_; }
    const std::string& name() const { return name_; }
    int capacity() const { return capacity_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setCapacity(int v) { capacity_ = v; }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Hall fromJson(const nlohmann::json& j);

private:
    std::string id_, centerId_, name_;
    int capacity_ = 0;
    bool active_ = true;
};

}  // namespace tdp
