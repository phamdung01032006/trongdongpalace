#pragma once

#include <string>

class SpecialRequest {
private:
    int id;
    int eventId;
    std::string description;
    std::string status;
    std::string note;

public:
    SpecialRequest(int id,
                    int eventId,
                    const std::string& description,
                    const std::string& status,
                    const std::string& note)
        : id(id),
          eventId(eventId),
          description(description),
          status(status),
          note(note) {}

    int getId() const { return id; }
    int getEventId() const { return eventId; }
    const std::string& getDescription() const { return description; }
    const std::string& getStatus() const { return status; }
    const std::string& getNote() const { return note; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
    void setNote(const std::string& newNote) { note = newNote; }
};