#pragma once

#include "models/Database.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace tdp {

class StorageError : public runtime_error {
public:
    explicit StorageError(const string& msg) : runtime_error(msg) {}
};

class JsonStorage {
public:
    explicit JsonStorage(filesystem::path dataDir, filesystem::path backupDir);

    void load();
    void save();
    void seedIfEmpty();

    Database& db() { return db_; }
    const Database& db() const { return db_; }
    const filesystem::path& dataDir() const { return dataDir_; }
    const string& lastError() const { return lastError_; }

private:
    filesystem::path dataDir_;
    filesystem::path backupDir_;
    Database db_;
    string lastError_;

    filesystem::path filePath(const string& name) const;
    nlohmann::json readJsonFile(const filesystem::path& path) const;
    void writeJsonAtomic(const filesystem::path& path, const nlohmann::json& j) const;
    void backupCurrentFiles() const;
    void writeAllFiles(const Database& data) const;
};

}  // namespace tdp
