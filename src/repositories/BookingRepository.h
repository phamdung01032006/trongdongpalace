#pragma once
#include "../database/DatabaseManager.h"
class BookingRepository { private: DatabaseManager& db; public: explicit BookingRepository(DatabaseManager& db) : db(db) {} };
