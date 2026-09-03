#pragma once
#include <string>
using namespace std;

class Center {
private:
    int id;
    string name;
    string address;
    string phone;

public:
    Center(int id,
            const string& name,
            const string& address,
            const string& phone)
            : id(id),
            name(name),
            address(address),
            phone(phone) {}

    // Getters
    int getId() const { return id; }
    const string& getName() const { return name; }
    const string& getAddress() const { return address; }
    const string& getPhone() const { return phone; }

    // Setters
    void setName(const string& newName) { name = newName; }
    void setAddress(const string& newAddress) { address = newAddress; }
    void setPhone(const string& newPhone) { phone = newPhone; }
};