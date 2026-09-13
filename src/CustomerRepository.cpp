#include "CustomerRepository.h"
#include "Exceptions.h"

#include <algorithm>

using namespace std;

CustomerRepository::CustomerRepository(JsonStorage& storage) : storage(storage) { load(); }

Customer CustomerRepository::fromJson(const json& item) {
    try {
        return Customer(item.at("id").get<string>(), item.at("name").get<string>(),
                        item.at("phone").get<string>(), item.value("email", ""),
                        item.value("address", ""));
    } catch (const json::exception& e) {
        throw FileException("Invalid customer data: " + string(e.what()));
    }
}

json CustomerRepository::toJson(const Customer& customer) {
    return {{"id", customer.getId()}, {"name", customer.getName()},
            {"phone", customer.getPhone()}, {"email", customer.getEmail()},
            {"address", customer.getAddress()}};
}

int CustomerRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < customers.size(); ++i) {
        if (customers[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

void CustomerRepository::load() {
    customers.clear();
    const json data = storage.load();
    for (const auto& item : data.at("customers")) customers.push_back(fromJson(item));
}

void CustomerRepository::save() {
    json data = storage.load();
    data["customers"] = json::array();
    for (const Customer& customer : customers) data["customers"].push_back(toJson(customer));
    storage.save(data);
}

void CustomerRepository::add(const Customer& customer) {
    if (findIndex(customer.getId()) >= 0) throw DuplicateException("Customer ID already exists: " + customer.getId());
    customers.push_back(customer);
    save();
}

void CustomerRepository::update(const Customer& customer) {
    const int index = findIndex(customer.getId());
    if (index < 0) throw NotFoundException("Customer not found: " + customer.getId());
    customers[index] = customer;
    save();
}

void CustomerRepository::remove(const string& id) {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Customer not found: " + id);
    customers.erase(customers.begin() + index);
    save();
}

Customer CustomerRepository::findById(const string& id) const {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Customer not found: " + id);
    return customers[index];
}

vector<Customer> CustomerRepository::getAll() const { return customers; }
