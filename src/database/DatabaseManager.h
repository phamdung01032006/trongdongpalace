#pragma once

#include <string>
#include <sqlite3.h>

// Bọc một kết nối SQLite duy nhất. Repository sẽ dùng class này để
// đọc/ghi dữ liệu; Service và Domain không được biết SQLite tồn tại.
//
// Ở giai đoạn này (Day 2) chỉ cần open()/close()/execute() để xác nhận
// kết nối được tới file database. Các hàm truy vấn có trả về dữ liệu
// (prepare/step/bind) sẽ được thêm khi Repository đầu tiên cần dùng.
class DatabaseManager {
public:
    explicit DatabaseManager(std::string dbPath);
    ~DatabaseManager();

    // Không cho copy: một sqlite3* connection không nên bị nhân bản.
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Mở kết nối tới file database. Ném std::runtime_error nếu thất bại.
    void open();

    // Đóng kết nối nếu đang mở. An toàn khi gọi nhiều lần.
    void close();

    // Thực thi một câu SQL không trả về dòng dữ liệu
    // (CREATE TABLE, INSERT, UPDATE, DELETE, ...).
    // Ném std::runtime_error nếu thất bại.
    void execute(const std::string& sql);

    bool isOpen() const;
    sqlite3* getConnection() const;

private:
    std::string dbPath;
    sqlite3* connection = nullptr;
};
