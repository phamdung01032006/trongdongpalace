#pragma once

#include "Event.h"

class CorporateEvent : public Event {
private:
    bool needsProjector;

public:
    CorporateEvent(const string& id, const string& customerId, const string& hallId,
                   const string& eventDate, const string& startTime, const string& endTime,
                   int guestCount, bool needsProjector = false);

    bool getNeedsProjector() const;
    void setNeedsProjector(bool needsProjector);
    string getEventType() const override;
    double calculateBasePrice() const override;
};
