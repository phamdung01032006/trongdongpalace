#include "JsonStorage.h"
#include "Exceptions.h"

#include <fstream>

using namespace std;

JsonStorage::JsonStorage(const filesystem::path& filePath) : filePath(filePath) {}

json JsonStorage::createEmptyData() {
    return {
        {"users", json::array()}, {"customers", json::array()},
        {"centers", json::array()}, {"halls", json::array()},
        {"events", json::array()}, {"bookings", json::array()},
        {"packages", json::array()}, {"services", json::array()},
        {"quotations", json::array()}, {"contracts", json::array()},
        {"payments", json::array()}, {"staffAssignments", json::array()},
        {"eventRequirements", json::array()}, {"tableAssignments", json::array()},
        {"incidents", json::array()}
    };
}

void JsonStorage::ensureSchema(json& data) {
    if (!data.is_object()) {
        throw FileException("Invalid JSON root. Expected an object.");
    }

    const json emptyData = createEmptyData();
    for (auto it = emptyData.begin(); it != emptyData.end(); ++it) {
        if (!data.contains(it.key())) {
            data[it.key()] = json::array();
        }
        if (!data[it.key()].is_array()) {
            throw FileException("Invalid JSON schema for '" + it.key() + "'.");
        }
    }
}

json JsonStorage::load() const {
    if (!filesystem::exists(filePath)) {
        save(createEmptyData());
    }

    ifstream input(filePath);
    if (!input) {
        throw FileException("Cannot open " + filePath.string() + ".");
    }

    try {
        json data;
        input >> data;
        ensureSchema(data);
        return data;
    } catch (const nlohmann::json::parse_error&) {
        throw FileException("Failed to parse " + filePath.filename().string() + ".");
    } catch (const nlohmann::json::exception& e) {
        throw FileException("Invalid JSON in " + filePath.filename().string() + ": " + e.what());
    }
}

void JsonStorage::save(const json& data) const {
    json checkedData = data;
    ensureSchema(checkedData);

    const filesystem::path parent = filePath.parent_path();
    if (!parent.empty()) {
        error_code error;
        filesystem::create_directories(parent, error);
        if (error) {
            throw FileException("Cannot create data directory: " + error.message());
        }
    }

    ofstream output(filePath);
    if (!output) {
        throw FileException("Cannot write " + filePath.string() + ".");
    }
    output << checkedData.dump(2) << '\n';
    if (!output) {
        throw FileException("Failed while writing " + filePath.string() + ".");
    }
}

const filesystem::path& JsonStorage::getFilePath() const {
    return filePath;
}
