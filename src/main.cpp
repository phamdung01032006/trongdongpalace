#include <iostream>

#include "database/DatabaseManager.h"
#include "database/DatabaseInitializer.h"

// Entry point. Ở giai đoạn này (Day 3) xác nhận kết nối SQLite và
// khởi tạo schema hoạt động đúng; chưa có chức năng nghiệp vụ.
int main() {
    DatabaseManager db("trong_dong_palace.db");

    try {
        db.open();
        std::cout << "Database connected successfully." << std::endl;

        DatabaseInitializer::initialize(db);
        std::cout << "Database schema initialized successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Database setup failed: " << ex.what() << std::endl;
        return 1;
    }

    db.close();
    return 0;
}
