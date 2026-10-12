#include "models/AuditLog.hpp"

namespace tdp {

AuditLog::AuditLog(std::string id, DateTime at, std::string username, std::string action, std::string detail)
    : id_(std::move(id)), username_(std::move(username)), action_(std::move(action)), detail_(std::move(detail)), at_(at) {}

nlohmann::json AuditLog::toJson() const {
    return {{"id", id_}, {"at", dateToJson(at_)}, {"username", username_}, {"action", action_}, {"detail", detail_}};
}

AuditLog AuditLog::fromJson(const nlohmann::json& j) {
    return AuditLog(j.at("id").get<std::string>(), dateFromJson(j.at("at")), j.value("username", ""),
                    j.value("action", ""), j.value("detail", ""));
}

}  // namespace tdp
