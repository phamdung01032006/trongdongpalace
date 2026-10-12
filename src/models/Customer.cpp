#include "models/Customer.hpp"

namespace tdp {

Customer::Customer(std::string id, std::string name, std::string phone, std::string email, std::string address,
                   std::string notes, DateTime createdAt)
    : id_(std::move(id)),
      name_(std::move(name)),
      phone_(std::move(phone)),
      email_(std::move(email)),
      address_(std::move(address)),
      notes_(std::move(notes)),
      createdAt_(createdAt) {}

nlohmann::json Customer::toJson() const {
    return {{"id", id_}, {"name", name_}, {"phone", phone_}, {"email", email_},
            {"address", address_}, {"notes", notes_}, {"createdAt", dateToJson(createdAt_)}};
}

Customer Customer::fromJson(const nlohmann::json& j) {
    return Customer(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.at("phone").get<std::string>(),
                    j.value("email", ""), j.value("address", ""), j.value("notes", ""),
                    dateFromJson(j.at("createdAt")));
}

}  // namespace tdp
