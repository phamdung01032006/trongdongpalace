#pragma once

// Customer KE THUA Person.
// Ngoai cac truong cua Person, Customer co them ma khach hang, ghi chu va
// ngay tao ho so.

#include "DateTime.h"
#include "Person.h"

#include <string>

using namespace std;

class Customer : public Person {
public:
    Customer(string id = "", string fullName = "", string phone = "", string email = "",
             string address = "", string notes = "");

    const string& get_id() const { return id_; }
    const string& get_notes() const { return notes_; }
    const DateTime& get_createdAt() const { return createdAt_; }

    void set_notes(const string& value) { notes_ = value; }
    void set_createdAt(const DateTime& value) { createdAt_ = value; }

    // Ghi de (override) ham ao cua Person.
    string roleName() const override { return "Khách hàng"; }
    string describe() const override;

    // Chuyen doi sang mot dong text de ghi file data/customers.txt.
    string toText() const;
    static Customer fromText(const string& line);

private:
    string id_;
    string notes_;
    DateTime createdAt_;
};
