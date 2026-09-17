#pragma once

#include "Center.h"
#include "JsonStorage.h"
#include "Repository.h"

class CenterRepository : public Repository<Center> {
private:
    JsonStorage& storage;
    vector<Center> centers;

    static Center fromJson(const json& item);
    static json toJson(const Center& center);
    int findIndex(const string& id) const;

public:
    explicit CenterRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Center& center) override;
    void update(const Center& center) override;
    void remove(const string& id) override;
    Center findById(const string& id) const override;
    vector<Center> getAll() const override;
};
