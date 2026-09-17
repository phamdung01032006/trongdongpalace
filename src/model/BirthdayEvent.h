#pragma once

#include "Event.h"

class BirthdayEvent : public Event {
private:
    int celebrantAge;

public:
    BirthdayEvent(const string& id, const string& customerId, const string& hallId,
                  const string& eventDate, const string& startTime, const string& endTime,
                  int guestCount, int celebrantAge);

    int getCelebrantAge() const;
    void setCelebrantAge(int celebrantAge);
    string getEventType() const override;
    double calculateBasePrice() const override;
};
