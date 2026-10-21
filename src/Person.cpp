#include "Person.h"

using namespace std;

Person::Person(string fullName, string phone, string email, string address)
    : fullName_(fullName), phone_(phone), email_(email), address_(address) {}

string Person::describe() const {
    if (phone_.empty()) return fullName_;
    return fullName_ + " - " + phone_;
}
