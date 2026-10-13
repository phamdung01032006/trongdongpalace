#include "models/Incident.hpp"

using namespace std;

namespace tdp {

Incident::Incident(string id, string eventId, string description, IncidentStatus status,
                   string resolution, DateTime createdAt)
    : id_(move(id)),
      eventId_(move(eventId)),
      description_(move(description)),
      resolution_(move(resolution)),
      status_(status),
      createdAt_(createdAt) {}

nlohmann::json Incident::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"description", description_}, {"status", toString(status_)},
            {"resolution", resolution_}, {"createdAt", dateToJson(createdAt_)}};
}

Incident Incident::fromJson(const nlohmann::json& j) {
    return Incident(j.at("id").get<string>(), j.at("eventId").get<string>(),
                    j.at("description").get<string>(), incidentStatusFromString(j.value("status", "Mo")),
                    j.value("resolution", ""), dateFromJson(j.at("createdAt")));
}

}  // namespace tdp
