#pragma once
#include "../database/DatabaseManager.h"
class PackageRepository { private: DatabaseManager& db; public: explicit PackageRepository(DatabaseManager& db) : db(db) {} };
