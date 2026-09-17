#include "Customer.h"
#include "Exceptions.h"

#include <cctype>

using namespace std;

namespace {
void requireValue(const string& value, const string& field) {
    if (value.empty()) throw ValidationException(field + " cannot be empty.");
}

bool validPhone(const string& phone) {
    int digitCount = 0;
    for (size_t i = 0; i < phone.size(); ++i) {
        if (isdigit(static_cast<unsigned char>(phone[i]))) {
            ++digitCount;
        } else if (!(i == 0 && phone[i] == '+')) {
            return false;
        }
    }
    return digitCount >= 9 && digitCount <= 15;
}
}

Customer::Customer(const string& id, const string& name, const string& phone,
                   const string& email, const string& address) : id(id) {
    requireValue(id, "Customer ID");
    setName(name);
    setPhone(phone);
    setEmail(email);
    setAddress(address);
}

const string& Customer::getId() const { return id; }
const string& Customer::getName() const { return name; }
const string& Customer::getPhone() const { return phone; }
const string& Customer::getEmail() const { return email; }
const string& Customer::getAddress() const { return address; }

void Customer::setName(const string& value) { requireValue(value, "Customer name"); name = value; }
void Customer::setPhone(const string& value) {
    if (!validPhone(value)) throw ValidationException("Phone number must contain 9 to 15 digits.");
    phone = value;
}
void Customer::setEmail(const string& value) {
    if (!value.empty() && (value.find('@') == string::npos || value.find('@') == 0)) {
        throw ValidationException("Email is invalid.");
    }
    email = value;
}
void Customer::setAddress(const string& value) { address = value; }
