#pragma once

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Hall {
public:
    Hall() = default;
    Hall(string id, string centerId, string name, int capacity, bool active);
    const string& id() const { return id_; }
    const string& centerId() const { return centerId_; }
    const string& name() const { return name_; }
    int capacity() const { return capacity_; }
    bool active() const { return active_; }
    void setName(string v) { name_ = move(v); }
    void setCapacity(int v) { capacity_ = v; }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Hall fromJson(const nlohmann::json& j);

private:
    string id_, centerId_, name_;
    int capacity_ = 0;
    bool active_ = true;
};

}  // namespace tdp
