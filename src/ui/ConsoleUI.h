#pragma once

#include "../database/DatabaseManager.h"

class ConsoleUI {
private:
    DatabaseManager& db;

public:
    explicit ConsoleUI(DatabaseManager& db);

    void start();
};