#pragma once
#include <string>
using namespace std;

enum class EventStatus {
    PLANNED,
    PREPARING,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

class Event {
private:
    int id;
    string eventCode;
    int customerId;
    string eventType;
    string eventDate; // ISO format, VD: "2026-09-20"
    string startTime; //VD: "18:00"
    string endTime; //VD: "22:00"
    int guestCount;
    EventStatus status;

public:
    Event(int id,
        const string& eventCode,
        int customerId,
        const string& eventType,
        const string& eventDate,
        const string& startTime,
        const string& endTime,
        int guestCount,
        EventStatus status)
        : id(id),
        eventCode(eventCode),
        customerId(customerId),
        eventType(eventType),
        eventDate(eventDate),
        startTime(startTime),
        endTime(endTime),
        guestCount(guestCount),
        status(status) {}
    
    // Getters
    int getId() const { return id; }
    const string& getEventCode() const { return eventCode; }
    int getCustomerId() const { return customerId; }
    const string& getEventType() const { return eventType; }
    const string& getEventDate() const { return eventDate; }
    const string& getStartTime() const { return startTime; }
    const string& getEndTime() const { return endTime; }
    int getGuestCount() const { return guestCount; }
    EventStatus getStatus() const { return status; }

    // Setters
    void setCustomerId(int newCustomerId) { customerId = newCustomerId; }
    void setEventType(const string& newEventType) { eventType = newEventType; }
    void setEventDate(const string& newEventDate) { eventDate = newEventDate; }
    void setStartTime(const string& newStartTime) { startTime = newStartTime; }
    void setEndTime(const string& newEndTime) { endTime = newEndTime; }
    void setGuestCount(int newGuestCount) { guestCount = newGuestCount; }
    void setStatus(EventStatus newStatus) { status = newStatus; }
};