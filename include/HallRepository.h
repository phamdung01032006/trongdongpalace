#pragma once

#include "Hall.h"
#include "JsonStorage.h"
#include "Repository.h"

class HallRepository : public Repository<Hall> {
private:
    JsonStorage& storage;
    vector<Hall> halls;

    static Hall fromJson(const json& item);
    static json toJson(const Hall& hall);
    int findIndex(const string& id) const;

public:
    explicit HallRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Hall& hall) override;
    void update(const Hall& hall) override;
    void remove(const string& id) override;
    Hall findById(const string& id) const override;
    vector<Hall> getAll() const override;
};
