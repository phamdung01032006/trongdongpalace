#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../database/DatabaseManager.h"
#include "../models/Event.h"

using namespace std;

class EventRepository {
private:
    DatabaseManager& db;
public:
    explicit EventRepository(DatabaseManager& db);
    void save(const Event& event);
    optional<Event> findByCode(const string& eventCode) const;
    vector<Event> findAll() const;
    void update(const Event& event);
};
