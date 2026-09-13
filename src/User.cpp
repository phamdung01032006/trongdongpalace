#include "User.h"
#include "Exceptions.h"

using namespace std;

namespace {
void requireValue(const string& value, const string& field) {
    if (value.empty()) throw ValidationException(field + " cannot be empty.");
}

bool validRole(const string& role) {
    return role == "ADMIN" || role == "SALES" || role == "ACCOUNTANT" ||
           role == "COORDINATOR" || role == "MANAGEMENT";
}
}

User::User(const string& id, const string& username, const string& password,
           const string& fullName, const string& role) : id(id) {
    requireValue(id, "User ID");
    setUsername(username);
    setPassword(password);
    setFullName(fullName);
    setRole(role);
}

const string& User::getId() const { return id; }
const string& User::getUsername() const { return username; }
const string& User::getPassword() const { return password; }
const string& User::getFullName() const { return fullName; }
const string& User::getRole() const { return role; }

void User::setUsername(const string& value) { requireValue(value, "Username"); username = value; }
void User::setPassword(const string& value) { requireValue(value, "Password"); password = value; }
void User::setFullName(const string& value) { requireValue(value, "Full name"); fullName = value; }
void User::setRole(const string& value) {
    if (!validRole(value)) throw ValidationException("Role is invalid.");
    role = value;
}
