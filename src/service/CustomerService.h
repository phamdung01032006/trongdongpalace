#pragma once

#include "CustomerRepository.h"

class CustomerService {
private:
    CustomerRepository& repository;

public:
    explicit CustomerService(CustomerRepository& repository);

    void create(const Customer& customer);
    vector<Customer> getAll() const;
    Customer findById(const string& id) const;
    void update(const Customer& customer);
    void remove(const string& id);
};
