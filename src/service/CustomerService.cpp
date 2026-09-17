#include "CustomerService.h"

CustomerService::CustomerService(CustomerRepository& repository) : repository(repository) {}

void CustomerService::create(const Customer& customer) { repository.add(customer); }
vector<Customer> CustomerService::getAll() const { return repository.getAll(); }
Customer CustomerService::findById(const string& id) const { return repository.findById(id); }
void CustomerService::update(const Customer& customer) { repository.update(customer); }
void CustomerService::remove(const string& id) { repository.remove(id); }
