#pragma once

#include "Contract.h"
#include "JsonStorage.h"
#include "Repository.h"

class ContractRepository : public Repository<Contract> {
private:
    JsonStorage& storage;
    vector<Contract> contracts;

    static Contract fromJson(const json& item);
    static json toJson(const Contract& contract);
    int findIndex(const string& id) const;

public:
    explicit ContractRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Contract& contract) override;
    void update(const Contract& contract) override;
    void remove(const string& id) override;
    Contract findById(const string& id) const override;
    vector<Contract> getAll() const override;
};
