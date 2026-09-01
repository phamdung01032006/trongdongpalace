#include <iostream>
using namespace std;

#include "database/DatabaseManager.h"

// Entry point. Ở giai đoạn này (Day 2) chỉ xác nhận kết nối SQLite
// hoạt động; chưa có chức năng nghiệp vụ.
int main() {
    DatabaseManager db("trong_dong_palace.db");

    try {
        db.open();
        cout << "Database connected successfully." << endl;
    } catch (const exception& ex) {
        cout << "Database connection failed: " << ex.what() << endl;
        return 1;
    }

    db.close();
    return 0;
}
