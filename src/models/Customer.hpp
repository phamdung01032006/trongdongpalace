#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Customer {
public:
    Customer() = default;
    Customer(std::string id, std::string name, std::string phone, std::string email, std::string address,
             std::string notes, DateTime createdAt);

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& phone() const { return phone_; }
    const std::string& email() const { return email_; }
    const std::string& address() const { return address_; }
    const std::string& notes() const { return notes_; }
    DateTime createdAt() const { return createdAt_; }

    void setName(std::string v) { name_ = std::move(v); }
    void setPhone(std::string v) { phone_ = std::move(v); }
    void setEmail(std::string v) { email_ = std::move(v); }
    void setAddress(std::string v) { address_ = std::move(v); }
    void setNotes(std::string v) { notes_ = std::move(v); }

    nlohmann::json toJson() const;
    static Customer fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, phone_, email_, address_, notes_;
    DateTime createdAt_;
};

}  // namespace tdp
