#pragma once

#include "JsonStorage.h"
#include "Payment.h"
#include "Repository.h"

class PaymentRepository : public Repository<Payment> {
private:
    JsonStorage& storage;
    vector<Payment> payments;

    static Payment fromJson(const json& item);
    static json toJson(const Payment& payment);
    int findIndex(const string& id) const;

public:
    explicit PaymentRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Payment& payment) override;
    void update(const Payment& payment) override;
    void remove(const string& id) override;
    Payment findById(const string& id) const override;
    vector<Payment> getAll() const override;
};
