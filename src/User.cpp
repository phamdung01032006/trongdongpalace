#include "User.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

User::User(string id, string username, string password, string fullName, string phone,
           string email, string address, Role role, string customerId, bool active)
    : Person(fullName, phone, email, address),
      id_(id),
      username_(username),
      password_(password),
      role_(role),
      customerId_(customerId),
      active_(active) {}

string User::describe() const {
    return "[" + id_ + "] " + fullName_ + " (" + username_ + " - " + roleName() + ")";
}

string User::toText() const {
    // id | username | password | ho ten | sdt | email | dia chi | vai tro | customerId | active
    vector<string> fields = {
        cleanField(id_),
        cleanField(username_),
        cleanField(password_),
        cleanField(fullName_),
        cleanField(phone_),
        cleanField(email_),
        cleanField(address_),
        cleanField(toString(role_)),
        cleanField(customerId_),
        boolText(active_)
    };
    return joinText(fields, '|');
}

User User::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 10) {
        throw FileException("Dong users.txt khong du truong: " + line);
    }

    return User(fieldAt(fields, 0), fieldAt(fields, 1), fieldAt(fields, 2),
                fieldAt(fields, 3), fieldAt(fields, 4), fieldAt(fields, 5),
                fieldAt(fields, 6), roleFromString(fieldAt(fields, 7)),
                fieldAt(fields, 8), toBool(fieldAt(fields, 9), true));
}
