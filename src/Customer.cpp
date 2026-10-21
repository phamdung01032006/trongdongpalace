#include "Customer.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Customer::Customer(string id, string fullName, string phone, string email, string address,
                   string notes)
    : Person(fullName, phone, email, address),
      id_(id),
      notes_(notes),
      createdAt_(DateTime::now()) {}

string Customer::describe() const {
    return "[" + id_ + "] " + fullName_ + " - " + phone_;
}

string Customer::toText() const {
    // Thu tu: id | ho ten | sdt | email | dia chi | ghi chu | ngay tao
    vector<string> fields = {
        cleanField(id_),
        cleanField(fullName_),
        cleanField(phone_),
        cleanField(email_),
        cleanField(address_),
        cleanField(notes_),
        cleanField(createdAt_.toString())
    };
    return joinText(fields, '|');
}

Customer Customer::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 7) {
        throw FileException("Dong customers.txt khong du truong: " + line);
    }

    Customer customer(fieldAt(fields, 0), fieldAt(fields, 1), fieldAt(fields, 2),
                      fieldAt(fields, 3), fieldAt(fields, 4), fieldAt(fields, 5));
    customer.set_createdAt(parseDateOr(fieldAt(fields, 6), DateTime::now()));
    return customer;
}
