#include "ServiceRepository.h"
#include "Exceptions.h"

using namespace std;

ServiceRepository::ServiceRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Service ServiceRepository::fromJson(const json& item) {
    try {
        return Service(
            item.at("id").get<string>(),
            item.at("name").get<string>(),
            item.at("unit").get<string>(),
            item.at("unitPrice").get<double>(),
            item.value("status", "")
        );
    } catch (const json::exception& e) {
        throw FileException("Invalid service data: " + string(e.what()));
    }
}

json ServiceRepository::toJson(const Service& service) {
    return {
        {"id", service.getId()},
        {"name", service.getName()},
        {"unit", service.getUnit()},
        {"unitPrice", service.getUnitPrice()},
        {"status", service.getStatus()}
    };
}

int ServiceRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < services.size(); ++i) {
        if (services[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

void ServiceRepository::load() {
    services.clear();

    const json data = storage.load();

    for (const auto& item : data.at("services"))
        services.push_back(fromJson(item));
}

void ServiceRepository::save() {
    json data = storage.load();

    data["services"] = json::array();

    for (const Service& service : services)
        data["services"].push_back(toJson(service));

    storage.save(data);
}

void ServiceRepository::add(const Service& service) {
    if (findIndex(service.getId()) >= 0)
        throw DuplicateException(
            "Service ID already exists: " + service.getId()
        );

    services.push_back(service);
    save();
}

void ServiceRepository::update(const Service& service) {
    const int index = findIndex(service.getId());

    if (index < 0)
        throw NotFoundException(
            "Service not found: " + service.getId()
        );

    services[index] = service;
    save();
}

void ServiceRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Service not found: " + id
        );

    services.erase(services.begin() + index);
    save();
}

Service ServiceRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Service not found: " + id
        );

    return services[index];
}

vector<Service> ServiceRepository::getAll() const {
    return services;
}