#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class AuditLog {
public:
    AuditLog() = default;
    AuditLog(std::string id, DateTime at, std::string username, std::string action, std::string detail);
    const std::string& id() const { return id_; }
    DateTime at() const { return at_; }
    const std::string& username() const { return username_; }
    const std::string& action() const { return action_; }
    const std::string& detail() const { return detail_; }
    nlohmann::json toJson() const;
    static AuditLog fromJson(const nlohmann::json& j);

private:
    std::string id_, username_, action_, detail_;
    DateTime at_;
};

}  // namespace tdp
