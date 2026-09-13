#include "CenterRepository.h"
#include "Exceptions.h"

using namespace std;

CenterRepository::CenterRepository(JsonStorage& storage) : storage(storage) { load(); }

Center CenterRepository::fromJson(const json& item) {
    try {
        return Center(item.at("id").get<string>(), item.at("name").get<string>(), item.value("address", ""));
    } catch (const json::exception& e) {
        throw FileException("Invalid center data: " + string(e.what()));
    }
}

json CenterRepository::toJson(const Center& center) {
    return {{"id", center.getId()}, {"name", center.getName()}, {"address", center.getAddress()}};
}

int CenterRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < centers.size(); ++i) {
        if (centers[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

void CenterRepository::load() {
    centers.clear();
    const json data = storage.load();
    for (const auto& item : data.at("centers")) centers.push_back(fromJson(item));
}

void CenterRepository::save() {
    json data = storage.load();
    data["centers"] = json::array();
    for (const Center& center : centers) data["centers"].push_back(toJson(center));
    storage.save(data);
}

void CenterRepository::add(const Center& center) {
    if (findIndex(center.getId()) >= 0) throw DuplicateException("Center ID already exists: " + center.getId());
    centers.push_back(center);
    save();
}

void CenterRepository::update(const Center& center) {
    const int index = findIndex(center.getId());
    if (index < 0) throw NotFoundException("Center not found: " + center.getId());
    centers[index] = center;
    save();
}

void CenterRepository::remove(const string& id) {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Center not found: " + id);
    centers.erase(centers.begin() + index);
    save();
}

Center CenterRepository::findById(const string& id) const {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Center not found: " + id);
    return centers[index];
}

vector<Center> CenterRepository::getAll() const { return centers; }
