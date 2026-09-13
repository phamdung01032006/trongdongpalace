#include "WeddingEvent.h"
#include "Exceptions.h"

using namespace std;

WeddingEvent::WeddingEvent(const string& id, const string& customerId, const string& hallId,
                           const string& eventDate, const string& startTime,
                           const string& endTime, int guestCount,
                           const string& ceremonyStyle)
    : Event(id, customerId, hallId, eventDate, startTime, endTime, guestCount),
      ceremonyStyle(ceremonyStyle) {
    if (ceremonyStyle.empty()) throw ValidationException("Ceremony style cannot be empty.");
}

const string& WeddingEvent::getCeremonyStyle() const { return ceremonyStyle; }
void WeddingEvent::setCeremonyStyle(const string& value) {
    if (value.empty()) throw ValidationException("Ceremony style cannot be empty.");
    ceremonyStyle = value;
}
string WeddingEvent::getEventType() const { return "WEDDING"; }
double WeddingEvent::calculateBasePrice() const {
    return 5000000.0 + getGuestCount() * 350000.0;
}
