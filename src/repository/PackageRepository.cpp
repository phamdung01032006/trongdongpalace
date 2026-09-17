#include "PackageRepository.h"
#include "Exceptions.h"

using namespace std;

PackageRepository::PackageRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Package PackageRepository::fromJson(const json& item) {
    try {
        return Package(
            item.at("id").get<string>(),
            item.at("name").get<string>(),
            item.value("description", ""),
            item.at("price").get<double>(),
            item.at("guestCapacity").get<int>(),
            item.value("status", "")
        );
    } catch (const json::exception& e) {
        throw FileException("Invalid package data: " + string(e.what()));
    }
}

json PackageRepository::toJson(const Package& package) {
    return {
        {"id", package.getId()},
        {"name", package.getName()},
        {"description", package.getDescription()},
        {"price", package.getPrice()},
        {"guestCapacity", package.getGuestCapacity()},
        {"status", package.getStatus()}
    };
}

int PackageRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < packages.size(); ++i) {
        if (packages[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

void PackageRepository::load() {
    packages.clear();

    const json data = storage.load();

    for (const auto& item : data.at("packages"))
        packages.push_back(fromJson(item));
}

void PackageRepository::save() {
    json data = storage.load();

    data["packages"] = json::array();

    for (const Package& package : packages)
        data["packages"].push_back(toJson(package));

    storage.save(data);
}

void PackageRepository::add(const Package& package) {
    if (findIndex(package.getId()) >= 0)
        throw DuplicateException(
            "Package ID already exists: " + package.getId()
        );

    packages.push_back(package);
    save();
}

void PackageRepository::update(const Package& package) {
    const int index = findIndex(package.getId());

    if (index < 0)
        throw NotFoundException(
            "Package not found: " + package.getId()
        );

    packages[index] = package;
    save();
}

void PackageRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Package not found: " + id
        );

    packages.erase(packages.begin() + index);
    save();
}

Package PackageRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Package not found: " + id
        );

    return packages[index];
}

vector<Package> PackageRepository::getAll() const {
    return packages;
}