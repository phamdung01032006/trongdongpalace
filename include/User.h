#pragma once

// User KE THUA Person.
// User la tai khoan dang nhap, co them username, password va vai tro.
// Tai khoan khach hang duoc lien ket voi mot ho so Customer qua customerId.

#include "Enums.h"
#include "Person.h"

#include <string>

using namespace std;

class User : public Person {
public:
    User(string id = "", string username = "", string password = "",
         string fullName = "", string phone = "", string email = "", string address = "",
         Role role = Role::Customer, string customerId = "", bool active = true);

    const string& get_id() const { return id_; }
    const string& get_username() const { return username_; }
    const string& get_password() const { return password_; }
    Role get_role() const { return role_; }
    bool is_active() const { return active_; }
    const string& get_customerId() const { return customerId_; }

    void set_password(const string& value) { password_ = value; }
    void set_role(Role value) { role_ = value; }
    void set_active(bool value) { active_ = value; }
    void set_customerId(const string& value) { customerId_ = value; }

    // Ghi de ham ao cua Person.
    string roleName() const override { return toString(role_); }
    string describe() const override;

    string toText() const;
    static User fromText(const string& line);

private:
    string id_;
    string username_;
    // LUU Y: do day la do an mon hoc, mat khau duoc luu dang ro trong
    // data/users.txt de sinh vien de doc/debug. He thong thuc te phai bam mat khau.
    string password_;
    Role role_ = Role::Customer;
    string customerId_;
    bool active_ = true;
};
