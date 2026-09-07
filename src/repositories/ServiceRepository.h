#pragma once
#include "../database/DatabaseManager.h"
class ServiceRepository { private: DatabaseManager& db; public: explicit ServiceRepository(DatabaseManager& db) : db(db) {} };
