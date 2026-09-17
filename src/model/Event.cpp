#include "Event.h"
#include "Exceptions.h"

#include <cctype>

using namespace std;

namespace {
bool validTime(const string& value) {
    return value.size() == 5 && value[2] == ':' &&
           isdigit(static_cast<unsigned char>(value[0])) &&
           isdigit(static_cast<unsigned char>(value[1])) &&
           isdigit(static_cast<unsigned char>(value[3])) &&
           isdigit(static_cast<unsigned char>(value[4])) &&
           stoi(value.substr(0, 2)) < 24 && stoi(value.substr(3, 2)) < 60;
}

void validateTimeRange(const string& startTime, const string& endTime) {
    if (!validTime(startTime) || !validTime(endTime)) {
        throw ValidationException("Time must use HH:MM format.");
    }
    if (startTime >= endTime) {
        throw ValidationException("Start time must be before end time.");
    }
}
}

Event::Event(const string& id, const string& customerId, const string& hallId,
             const string& eventDate, const string& startTime, const string& endTime,
             int guestCount, const string& status)
    : id(id), customerId(customerId) {
    if (id.empty()) throw ValidationException("Event ID cannot be empty.");
    if (customerId.empty()) throw ValidationException("Customer ID cannot be empty.");
    setHallId(hallId);
    setEventDate(eventDate);
    setTimeRange(startTime, endTime);
    setGuestCount(guestCount);
    setStatus(status);
}

const string& Event::getId() const { return id; }
const string& Event::getCustomerId() const { return customerId; }
const string& Event::getHallId() const { return hallId; }
const string& Event::getEventDate() const { return eventDate; }
const string& Event::getStartTime() const { return startTime; }
const string& Event::getEndTime() const { return endTime; }
int Event::getGuestCount() const { return guestCount; }
const string& Event::getStatus() const { return status; }

void Event::setHallId(const string& value) {
    if (value.empty()) throw ValidationException("Hall ID cannot be empty.");
    hallId = value;
}
void Event::setEventDate(const string& value) {
    if (value.empty()) throw ValidationException("Event date cannot be empty.");
    eventDate = value;
}
void Event::setTimeRange(const string& newStartTime, const string& newEndTime) {
    validateTimeRange(newStartTime, newEndTime);
    startTime = newStartTime;
    endTime = newEndTime;
}
void Event::setGuestCount(int value) {
    if (value <= 0) throw ValidationException("Guest count must be greater than 0.");
    guestCount = value;
}
void Event::setStatus(const string& value) {
    if (value.empty()) throw ValidationException("Event status cannot be empty.");
    status = value;
}
