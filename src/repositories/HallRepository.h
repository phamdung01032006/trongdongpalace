#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../database/DatabaseManager.h"
#include "../models/Hall.h"

using namespace std;

class HallRepository {
private:
    DatabaseManager& db;
public:
    explicit HallRepository(DatabaseManager& db);
    void save(const Hall& hall);
    optional<Hall> findByCode(const string& hallCode) const;
    vector<Hall> findAll() const;
    void update(const Hall& hall);
};
