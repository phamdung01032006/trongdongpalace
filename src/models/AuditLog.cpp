#include "models/AuditLog.hpp"

using namespace std;

namespace tdp {

AuditLog::AuditLog(string id, DateTime at, string username, string action, string detail)
    : id_(move(id)), username_(move(username)), action_(move(action)), detail_(move(detail)), at_(at) {}

nlohmann::json AuditLog::toJson() const {
    return {{"id", id_}, {"at", dateToJson(at_)}, {"username", username_}, {"action", action_}, {"detail", detail_}};
}

AuditLog AuditLog::fromJson(const nlohmann::json& j) {
    return AuditLog(j.at("id").get<string>(), dateFromJson(j.at("at")), j.value("username", ""),
                    j.value("action", ""), j.value("detail", ""));
}

}  // namespace tdp
