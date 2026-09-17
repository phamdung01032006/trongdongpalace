#include <iostream>
#include <ui/ConsoleUI.h>
#include "database/DatabaseManager.h"
#include "database/DatabaseInitializer.h"
using namespace std;

int main() {

    try {
        cout << "Application started.\n";

        DatabaseManager db("trong_dong_palace.db");
        db.open();
        cout << "Database connected successfully." << endl;

        DatabaseInitializer::initialize(db);   // gọi static method trực tiếp qua tên class
        cout << "Database schema initialized successfully." << endl;

        ConsoleUI app(db);
        app.start();

        db.close();
    } catch (const exception& ex) {
        cout << "Fatal Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}