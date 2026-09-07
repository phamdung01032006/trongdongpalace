#pragma once

#include <string>
#include <vector>
#include "../models/Customer.h"
#include "../repositories/CustomerRepository.h"

using namespace std;

class CustomerService {
private:
    CustomerRepository& repository;
    void validate(const Customer& customer) const;
public:
    explicit CustomerService(CustomerRepository& repository);
    void create(const string& code, const string& name, const string& phone, const string& email, const string& address);
    Customer findByCode(const string& code) const;
    vector<Customer> findAll() const;
    void update(const string& code, const string& name, const string& phone, const string& email, const string& address);
    void remove(const string& code);
};
