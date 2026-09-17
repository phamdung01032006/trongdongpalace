#pragma once
#include <string>
using namespace std;
class Booking {
    string id, eventId, packageId, status;
public:
    Booking(const string& id, const string& eventId, const string& packageId, const string& status = "PENDING");
    const string& getId() const; const string& getEventId() const; const string& getPackageId() const; const string& getStatus() const;
};
