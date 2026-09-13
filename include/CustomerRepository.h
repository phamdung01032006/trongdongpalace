#pragma once

#include "Customer.h"
#include "JsonStorage.h"
#include "Repository.h"

class CustomerRepository : public Repository<Customer> {
private:
    JsonStorage& storage;
    vector<Customer> customers;

    static Customer fromJson(const json& item);
    static json toJson(const Customer& customer);
    int findIndex(const string& id) const;

public:
    explicit CustomerRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Customer& customer) override;
    void update(const Customer& customer) override;
    void remove(const string& id) override;
    Customer findById(const string& id) const override;
    vector<Customer> getAll() const override;
};
