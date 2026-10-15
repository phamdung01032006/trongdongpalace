#ifndef TDP_MODELS_CUSTOMER_CPP
#define TDP_MODELS_CUSTOMER_CPP

#include "models/Enums.cpp"

#include "utils/text_fields.cpp"
#include <string>

using namespace std;

class Customer {
public:
    Customer() = default;

    Customer(const string& id, const string& name, const string& phone,
             const string& email, const string& address, const string& notes, DateTime createdAt)
        : id_(id),
          name_(name),
          phone_(phone),
          email_(email),
          address_(address),
          notes_(notes),
          createdAt_(createdAt) {}

    const string& id() const { return id_; }
    const string& name() const { return name_; }
    const string& phone() const { return phone_; }
    const string& email() const { return email_; }
    const string& address() const { return address_; }
    const string& notes() const { return notes_; }
    int age() const { return age_; }
    void setAge(int v) { age_ = v; }
    DateTime createdAt() const { return createdAt_; }

    void setName(const string& value) { name_ = value; }
    void setPhone(const string& value) { phone_ = value; }
    void setEmail(const string& value) { email_ = value; }
    void setAddress(const string& value) { address_ = value; }
    void setNotes(const string& value) { notes_ = value; }

    vector<string> toFields() const;
    static Customer fromFields(const vector<string>& f);

private:
    string id_, name_, phone_, email_, address_, notes_;
    DateTime createdAt_;
    int age_ = 0;
};

vector<string> Customer::toFields() const {
    vector<string> f = {id_, name_, phone_, email_, address_, notes_, createdAt_.toString(), to_string(age_)};
    return f;
}

Customer Customer::fromFields(const vector<string>& f) {
    requireFieldCount(f, 8);
    Customer item;
    item.id_ = f.at(0);
    item.name_ = f.at(1);
    item.phone_ = f.at(2);
    item.email_ = f.at(3);
    item.address_ = f.at(4);
    item.notes_ = f.at(5);
    item.createdAt_ = textDate(f.at(6));
    item.age_ = textInt(f.at(7));
    return item;
}

#endif
