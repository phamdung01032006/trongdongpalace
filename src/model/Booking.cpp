#include "Booking.h"
#include "Exceptions.h"
Booking::Booking(const string& id, const string& eventId, const string& packageId, const string& status) : id(id), eventId(eventId), packageId(packageId), status(status) {
    if (id.empty() || eventId.empty() || packageId.empty()) throw ValidationException("Booking ID, event ID, and package ID cannot be empty.");
}
const string& Booking::getId() const { return id; } const string& Booking::getEventId() const { return eventId; } const string& Booking::getPackageId() const { return packageId; } const string& Booking::getStatus() const { return status; }
