#pragma once

#include <string>

using namespace std;

class Package {
private:
    string id;
    string name;
    string description;
    double price;
    int guestCapacity;
    string status;

public:
    Package(const string& id, const string& name, const string& description,
            double price, int guestCapacity, const string& status = "ACTIVE");

    const string& getId() const;
    const string& getName() const;
    const string& getDescription() const;
    double getPrice() const;
    int getGuestCapacity() const;
    const string& getStatus() const;

    void setName(const string& name);
    void setDescription(const string& description);
    void setPrice(double price);
    void setGuestCapacity(int guestCapacity);
    void setStatus(const string& status);
};
