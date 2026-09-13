#include "HallRepository.h"
#include "Exceptions.h"

using namespace std;

HallRepository::HallRepository(JsonStorage& storage) : storage(storage) { load(); }

Hall HallRepository::fromJson(const json& item) {
    try {
        return Hall(item.at("id").get<string>(), item.at("centerId").get<string>(),
                    item.at("name").get<string>(), item.at("capacity").get<int>(),
                    item.value("status", "ACTIVE"));
    } catch (const json::exception& e) {
        throw FileException("Invalid hall data: " + string(e.what()));
    }
}

json HallRepository::toJson(const Hall& hall) {
    return {{"id", hall.getId()}, {"centerId", hall.getCenterId()}, {"name", hall.getName()},
            {"capacity", hall.getCapacity()}, {"status", hall.getStatus()}};
}

int HallRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < halls.size(); ++i) {
        if (halls[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

void HallRepository::load() {
    halls.clear();
    const json data = storage.load();
    for (const auto& item : data.at("halls")) halls.push_back(fromJson(item));
}

void HallRepository::save() {
    json data = storage.load();
    data["halls"] = json::array();
    for (const Hall& hall : halls) data["halls"].push_back(toJson(hall));
    storage.save(data);
}

void HallRepository::add(const Hall& hall) {
    if (findIndex(hall.getId()) >= 0) throw DuplicateException("Hall ID already exists: " + hall.getId());
    halls.push_back(hall);
    save();
}

void HallRepository::update(const Hall& hall) {
    const int index = findIndex(hall.getId());
    if (index < 0) throw NotFoundException("Hall not found: " + hall.getId());
    halls[index] = hall;
    save();
}

void HallRepository::remove(const string& id) {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Hall not found: " + id);
    halls.erase(halls.begin() + index);
    save();
}

Hall HallRepository::findById(const string& id) const {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Hall not found: " + id);
    return halls[index];
}

vector<Hall> HallRepository::getAll() const { return halls; }
