#pragma once
#include "../database/DatabaseManager.h"
class QuotationRepository { private: DatabaseManager& db; public: explicit QuotationRepository(DatabaseManager& db) : db(db) {} };
