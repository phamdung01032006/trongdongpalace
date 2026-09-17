#include "BirthdayEvent.h"
#include "Exceptions.h"

using namespace std;

BirthdayEvent::BirthdayEvent(const string& id, const string& customerId, const string& hallId,
                             const string& eventDate, const string& startTime,
                             const string& endTime, int guestCount, int celebrantAge)
    : Event(id, customerId, hallId, eventDate, startTime, endTime, guestCount) {
    setCelebrantAge(celebrantAge);
}

int BirthdayEvent::getCelebrantAge() const { return celebrantAge; }
void BirthdayEvent::setCelebrantAge(int value) {
    if (value <= 0) throw ValidationException("Celebrant age must be greater than 0.");
    celebrantAge = value;
}
string BirthdayEvent::getEventType() const { return "BIRTHDAY"; }
double BirthdayEvent::calculateBasePrice() const {
    return 2000000.0 + getGuestCount() * 200000.0;
}
