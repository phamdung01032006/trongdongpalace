#include "models/Customer.hpp"

using namespace std;

namespace tdp {

Customer::Customer(string id, string name, string phone, string email, string address,
                   string notes, DateTime createdAt)
    : id_(move(id)),
      name_(move(name)),
      phone_(move(phone)),
      email_(move(email)),
      address_(move(address)),
      notes_(move(notes)),
      createdAt_(createdAt) {}

nlohmann::json Customer::toJson() const {
    return {{"id", id_}, {"name", name_}, {"phone", phone_}, {"email", email_},
            {"address", address_}, {"notes", notes_}, {"createdAt", dateToJson(createdAt_)}};
}

Customer Customer::fromJson(const nlohmann::json& j) {
    return Customer(j.at("id").get<string>(), j.at("name").get<string>(), j.at("phone").get<string>(),
                    j.value("email", ""), j.value("address", ""), j.value("notes", ""),
                    dateFromJson(j.at("createdAt")));
}

}  // namespace tdp
