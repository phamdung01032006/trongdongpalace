#include "DatabaseManager.h"

#include <stdexcept>
#include <utility>

DatabaseManager::DatabaseManager(std::string dbPath)
    : dbPath(std::move(dbPath)) {}

DatabaseManager::~DatabaseManager() {
    close();
}

void DatabaseManager::open() {
    int result = sqlite3_open(dbPath.c_str(), &connection);

    if (result != SQLITE_OK) {
        std::string message = "Unable to open SQLite database: ";
        message += sqlite3_errmsg(connection);

        sqlite3_close(connection);
        connection = nullptr;

        throw std::runtime_error(message);
    }

    // SQLite không tự enforce FOREIGN KEY trừ khi bật pragma này cho
    // từng connection. Bật ngay sau khi mở để mọi FOREIGN KEY khai báo
    // trong schema (Day 3) thực sự có hiệu lực.
    execute("PRAGMA foreign_keys = ON;");
}

void DatabaseManager::close() {
    if (connection != nullptr) {
        sqlite3_close(connection);
        connection = nullptr;
    }
}

void DatabaseManager::execute(const std::string& sql) {
    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        connection,
        sql.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        std::string message = "Failed to execute SQL: ";
        message += (errorMessage != nullptr) ? errorMessage : "unknown error";

        sqlite3_free(errorMessage);

        throw std::runtime_error(message);
    }
}

bool DatabaseManager::isOpen() const {
    return connection != nullptr;
}

sqlite3* DatabaseManager::getConnection() const {
    return connection;
}
