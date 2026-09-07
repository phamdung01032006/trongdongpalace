#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../database/DatabaseManager.h"
#include "../models/Customer.h"

using namespace std;

class CustomerRepository {
private:
    DatabaseManager& db;

public:
    explicit CustomerRepository(DatabaseManager& db);
    void save(const Customer& customer);
    optional<Customer> findByCode(const string& customerCode) const;
    vector<Customer> findAll() const;
    void update(const Customer& customer);
    void remove(const string& customerCode);
    bool isUsedByEvent(const string& customerCode) const;
};
