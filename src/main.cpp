#include <iostream>

#include "database/DatabaseManager.h"

// Entry point. Ở giai đoạn này (Day 2) chỉ xác nhận kết nối SQLite
// hoạt động; chưa có chức năng nghiệp vụ.
int main() {
    DatabaseManager db("trong_dong_palace.db");

    try {
        db.open();
        std::cout << "Database connected successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Database connection failed: " << ex.what() << std::endl;
        return 1;
    }

    db.close();
    return 0;
}
