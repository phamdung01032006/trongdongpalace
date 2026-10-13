#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Incident {
public:
    Incident() = default;
    Incident(string id, string eventId, string description, IncidentStatus status,
             string resolution, DateTime createdAt);
    const string& id() const { return id_; }
    const string& eventId() const { return eventId_; }
    const string& description() const { return description_; }
    IncidentStatus status() const { return status_; }
    const string& resolution() const { return resolution_; }
    DateTime createdAt() const { return createdAt_; }
    void setStatus(IncidentStatus v) { status_ = v; }
    void setResolution(string v) { resolution_ = move(v); }
    nlohmann::json toJson() const;
    static Incident fromJson(const nlohmann::json& j);

private:
    string id_, eventId_, description_, resolution_;
    IncidentStatus status_ = IncidentStatus::Mo;
    DateTime createdAt_;
};

}  // namespace tdp
