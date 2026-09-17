#pragma once
#include <string>
using namespace std;
class Quotation {
    string id, eventId, status; double total;
public:
    Quotation(const string& id, const string& eventId, double total, const string& status = "DRAFT");
    const string& getId() const; const string& getEventId() const; double getTotal() const; const string& getStatus() const;
};
