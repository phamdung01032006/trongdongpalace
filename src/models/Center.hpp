#pragma once

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Center {
public:
    Center() = default;
    Center(string id, string name, string address, bool active);
    const string& id() const { return id_; }
    const string& name() const { return name_; }
    const string& address() const { return address_; }
    bool active() const { return active_; }
    void setName(string v) { name_ = move(v); }
    void setAddress(string v) { address_ = move(v); }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Center fromJson(const nlohmann::json& j);

private:
    string id_, name_, address_;
    bool active_ = true;
};

}  // namespace tdp
