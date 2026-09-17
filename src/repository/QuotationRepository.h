#pragma once

#include "JsonStorage.h"
#include "Quotation.h"
#include "Repository.h"

class QuotationRepository : public Repository<Quotation> {
private:
    JsonStorage& storage;
    vector<Quotation> quotations;

    static Quotation fromJson(const json& item);
    static json toJson(const Quotation& quotation);
    int findIndex(const string& id) const;

public:
    explicit QuotationRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Quotation& quotation) override;
    void update(const Quotation& quotation) override;
    void remove(const string& id) override;
    Quotation findById(const string& id) const override;
    vector<Quotation> getAll() const override;
};
