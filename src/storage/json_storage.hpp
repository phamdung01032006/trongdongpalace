#pragma once

#include "models/entities.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace tdp {

class StorageError : public std::runtime_error {
public:
    explicit StorageError(const std::string& msg) : std::runtime_error(msg) {}
};

class JsonStorage {
public:
    explicit JsonStorage(std::filesystem::path dataDir, std::filesystem::path backupDir);

    void load();
    void save();
    void seedIfEmpty();

    Database& db() { return db_; }
    const Database& db() const { return db_; }
    const std::filesystem::path& dataDir() const { return dataDir_; }
    const std::string& lastError() const { return lastError_; }

private:
    std::filesystem::path dataDir_;
    std::filesystem::path backupDir_;
    Database db_;
    std::string lastError_;

    std::filesystem::path filePath(const std::string& name) const;
    nlohmann::json readJsonFile(const std::filesystem::path& path) const;
    void writeJsonAtomic(const std::filesystem::path& path, const nlohmann::json& j) const;
    void backupCurrentFiles() const;
    void writeAllFiles(const Database& data) const;
};

}  // namespace tdp
