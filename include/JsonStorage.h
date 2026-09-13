#pragma once

#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class JsonStorage {
private:
    filesystem::path filePath;

    static json createEmptyData();
    static void ensureSchema(json& data);

public:
    explicit JsonStorage(const filesystem::path& filePath = "data/data.json");

    json load() const;
    void save(const json& data) const;
    const filesystem::path& getFilePath() const;
};
