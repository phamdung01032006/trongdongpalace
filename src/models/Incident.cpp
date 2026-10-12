#include "models/Incident.hpp"

namespace tdp {

Incident::Incident(std::string id, std::string eventId, std::string description, IncidentStatus status,
                   std::string resolution, DateTime createdAt)
    : id_(std::move(id)),
      eventId_(std::move(eventId)),
      description_(std::move(description)),
      resolution_(std::move(resolution)),
      status_(status),
      createdAt_(createdAt) {}

nlohmann::json Incident::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"description", description_}, {"status", toString(status_)},
            {"resolution", resolution_}, {"createdAt", dateToJson(createdAt_)}};
}

Incident Incident::fromJson(const nlohmann::json& j) {
    return Incident(j.at("id").get<std::string>(), j.at("eventId").get<std::string>(),
                    j.at("description").get<std::string>(), incidentStatusFromString(j.value("status", "Mo")),
                    j.value("resolution", ""), dateFromJson(j.at("createdAt")));
}

}  // namespace tdp
