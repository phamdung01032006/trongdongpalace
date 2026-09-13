#pragma once

#include <string>

using namespace std;

class Service {
private:
    string id;
    string name;
    string unit;
    double unitPrice;
    string status;

public:
    Service(const string& id, const string& name, const string& unit,
            double unitPrice, const string& status = "ACTIVE");

    const string& getId() const;
    const string& getName() const;
    const string& getUnit() const;
    double getUnitPrice() const;
    const string& getStatus() const;

    void setName(const string& name);
    void setUnit(const string& unit);
    void setUnitPrice(double unitPrice);
    void setStatus(const string& status);
};
