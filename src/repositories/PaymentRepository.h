#pragma once
#include "../database/DatabaseManager.h"
class PaymentRepository { private: DatabaseManager& db; public: explicit PaymentRepository(DatabaseManager& db) : db(db) {} };
