#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class AuditLog {
public:
    AuditLog() = default;
    AuditLog(string id, DateTime at, string username, string action, string detail);
    const string& id() const { return id_; }
    DateTime at() const { return at_; }
    const string& username() const { return username_; }
    const string& action() const { return action_; }
    const string& detail() const { return detail_; }
    nlohmann::json toJson() const;
    static AuditLog fromJson(const nlohmann::json& j);

private:
    string id_, username_, action_, detail_;
    DateTime at_;
};

}  // namespace tdp
