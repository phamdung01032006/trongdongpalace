#pragma once

#include <string>

using namespace std;

class Customer {
private:
    string id;
    string name;
    string phone;
    string email;
    string address;

public:
    Customer(const string& id, const string& name, const string& phone,
             const string& email = "", const string& address = "");

    const string& getId() const;
    const string& getName() const;
    const string& getPhone() const;
    const string& getEmail() const;
    const string& getAddress() const;

    void setName(const string& name);
    void setPhone(const string& phone);
    void setEmail(const string& email);
    void setAddress(const string& address);
};
