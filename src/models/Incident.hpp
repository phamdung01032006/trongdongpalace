#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Incident {
public:
    Incident() = default;
    Incident(std::string id, std::string eventId, std::string description, IncidentStatus status,
             std::string resolution, DateTime createdAt);
    const std::string& id() const { return id_; }
    const std::string& eventId() const { return eventId_; }
    const std::string& description() const { return description_; }
    IncidentStatus status() const { return status_; }
    const std::string& resolution() const { return resolution_; }
    DateTime createdAt() const { return createdAt_; }
    void setStatus(IncidentStatus v) { status_ = v; }
    void setResolution(std::string v) { resolution_ = std::move(v); }
    nlohmann::json toJson() const;
    static Incident fromJson(const nlohmann::json& j);

private:
    std::string id_, eventId_, description_, resolution_;
    IncidentStatus status_ = IncidentStatus::Mo;
    DateTime createdAt_;
};

}  // namespace tdp
