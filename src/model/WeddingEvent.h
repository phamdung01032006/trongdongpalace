#pragma once

#include "Event.h"

class WeddingEvent : public Event {
private:
    string ceremonyStyle;

public:
    WeddingEvent(const string& id, const string& customerId, const string& hallId,
                 const string& eventDate, const string& startTime, const string& endTime,
                 int guestCount, const string& ceremonyStyle = "STANDARD");

    const string& getCeremonyStyle() const;
    void setCeremonyStyle(const string& ceremonyStyle);
    string getEventType() const override;
    double calculateBasePrice() const override;
};
