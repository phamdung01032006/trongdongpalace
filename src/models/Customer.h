#pragma once
#include <string>
using namespace std;

class Customer {
private:
    int id;
    string customerCode;
    string name;
    string phone;
    string email;
    string address;

public:
    Customer(int id,
            const string& customerCode,
            const string& name,
            const string& phone,
            const string& email,
            const string& address )
            : id(id),
            customerCode(customerCode),
            name(name),
            phone(phone),
            email(email),
            address(address) {}

    // Getters
    int getId() const { return id; }
    const string& getCustomerCode() const { return customerCode; }
    const string& getName() const { return name; }
    const string& getPhone() const { return phone; }
    const string& getEmail() const { return email; }
    const string& getAddress() const { return address; }

    // Setters
    void setName(const string& newName) { name = newName; }
    void setPhone(const string& newPhone) { phone = newPhone; }
    void setEmail(const string& newEmail) { email = newEmail; }
    void setAddress(const string& newAddress) { address = newAddress; }
};