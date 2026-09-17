#include "CorporateEvent.h"

using namespace std;

CorporateEvent::CorporateEvent(const string& id, const string& customerId, const string& hallId,
                               const string& eventDate, const string& startTime,
                               const string& endTime, int guestCount, bool needsProjector)
    : Event(id, customerId, hallId, eventDate, startTime, endTime, guestCount),
      needsProjector(needsProjector) {}

bool CorporateEvent::getNeedsProjector() const { return needsProjector; }
void CorporateEvent::setNeedsProjector(bool value) { needsProjector = value; }
string CorporateEvent::getEventType() const { return "CORPORATE"; }
double CorporateEvent::calculateBasePrice() const {
    const double projectorFee = needsProjector ? 1500000.0 : 0.0;
    return 3000000.0 + getGuestCount() * 250000.0 + projectorFee;
}
