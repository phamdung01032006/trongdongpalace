#pragma once

#include <string>

class Incident {
private:
    int id;
    int eventId;
    std::string description;
    std::string severity;
    std::string status;
    std::string resolution;

public:
    Incident(int id,
             int eventId,
             const std::string& description,
             const std::string& severity,
             const std::string& status,
             const std::string& resolution)
        : id(id),
          eventId(eventId),
          description(description),
          severity(severity),
          status(status),
          resolution(resolution) {}

    int getId() const { return id; }
    int getEventId() const { return eventId; }
    const std::string& getDescription() const { return description; }
    const std::string& getSeverity() const { return severity; }
    const std::string& getStatus() const { return status; }
    const std::string& getResolution() const { return resolution; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setSeverity(const std::string& newSeverity) { severity = newSeverity; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
    void setResolution(const std::string& newResolution) { resolution = newResolution; }
};