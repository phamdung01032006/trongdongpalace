#pragma once

#include <string>

using namespace std;

class Event {
private:
    string id;
    string customerId;
    string hallId;
    string eventDate;
    string startTime;
    string endTime;
    int guestCount;
    string status;

protected:
    Event(const string& id, const string& customerId, const string& hallId,
          const string& eventDate, const string& startTime, const string& endTime,
          int guestCount, const string& status = "PLANNED");

public:
    virtual ~Event() = default;

    const string& getId() const;
    const string& getCustomerId() const;
    const string& getHallId() const;
    const string& getEventDate() const;
    const string& getStartTime() const;
    const string& getEndTime() const;
    int getGuestCount() const;
    const string& getStatus() const;

    void setHallId(const string& hallId);
    void setEventDate(const string& eventDate);
    void setTimeRange(const string& startTime, const string& endTime);
    void setGuestCount(int guestCount);
    void setStatus(const string& status);

    virtual string getEventType() const = 0;
    virtual double calculateBasePrice() const = 0;
};
