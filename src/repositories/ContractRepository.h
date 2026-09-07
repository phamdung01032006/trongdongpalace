#pragma once
#include "../database/DatabaseManager.h"
class ContractRepository { private: DatabaseManager& db; public: explicit ContractRepository(DatabaseManager& db) : db(db) {} };
