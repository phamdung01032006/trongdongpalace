#pragma once

#include "JsonStorage.h"
#include "Package.h"
#include "Repository.h"

class PackageRepository : public Repository<Package> {
private:
    JsonStorage& storage;
    vector<Package> packages;

    static Package fromJson(const json& item);
    static json toJson(const Package& package);
    int findIndex(const string& id) const;

public:
    explicit PackageRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Package& package) override;
    void update(const Package& package) override;
    void remove(const string& id) override;
    Package findById(const string& id) const override;
    vector<Package> getAll() const override;
};
