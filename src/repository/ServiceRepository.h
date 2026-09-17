#pragma once

#include "JsonStorage.h"
#include "Repository.h"
#include "Service.h"

class ServiceRepository : public Repository<Service> {
private:
    JsonStorage& storage;
    vector<Service> services;

    static Service fromJson(const json& item);
    static json toJson(const Service& service);
    int findIndex(const string& id) const;

public:
    explicit ServiceRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Service& service) override;
    void update(const Service& service) override;
    void remove(const string& id) override;
    Service findById(const string& id) const override;
    vector<Service> getAll() const override;
};
