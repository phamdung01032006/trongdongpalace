#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Customer {
public:
    Customer() = default;
    Customer(string id, string name, string phone, string email, string address,
             string notes, DateTime createdAt);

    const string& id() const { return id_; }
    const string& name() const { return name_; }
    const string& phone() const { return phone_; }
    const string& email() const { return email_; }
    const string& address() const { return address_; }
    const string& notes() const { return notes_; }
    DateTime createdAt() const { return createdAt_; }

    void setName(string v) { name_ = move(v); }
    void setPhone(string v) { phone_ = move(v); }
    void setEmail(string v) { email_ = move(v); }
    void setAddress(string v) { address_ = move(v); }
    void setNotes(string v) { notes_ = move(v); }

    nlohmann::json toJson() const;
    static Customer fromJson(const nlohmann::json& j);

private:
    string id_, name_, phone_, email_, address_, notes_;
    DateTime createdAt_;
};

}  // namespace tdp
