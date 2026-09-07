#include "CustomerService.h"
#include "../exceptions/AppException.h"
#include <cctype>

CustomerService::CustomerService(CustomerRepository& repository) : repository(repository) {}

void CustomerService::validate(const Customer& customer) const {
    if (customer.getCustomerCode().empty()) throw ValidationException("Customer ID is required.");
    if (customer.getName().empty()) throw ValidationException("Customer name is required.");
    const string& phone = customer.getPhone();
    if (phone.length() < 9 || phone.length() > 11) throw ValidationException("Phone number is invalid.");
    for (char character : phone) if (!isdigit(static_cast<unsigned char>(character))) throw ValidationException("Phone number is invalid.");
}

void CustomerService::create(const string& code, const string& name, const string& phone, const string& email, const string& address) {
    Customer customer(0, code, name, phone, email, address);
    validate(customer);
    if (repository.findByCode(code).has_value()) throw BusinessException("Customer ID already exists.");
    repository.save(customer);
}
Customer CustomerService::findByCode(const string& code) const { auto customer = repository.findByCode(code); if (!customer.has_value()) throw EntityNotFoundException("Customer not found."); return *customer; }
vector<Customer> CustomerService::findAll() const { return repository.findAll(); }
void CustomerService::update(const string& code, const string& name, const string& phone, const string& email, const string& address) { Customer customer = findByCode(code); customer.setName(name); customer.setPhone(phone); customer.setEmail(email); customer.setAddress(address); validate(customer); repository.update(customer); }
void CustomerService::remove(const string& code) { findByCode(code); if (repository.isUsedByEvent(code)) throw BusinessException("Customer cannot be deleted because it is used by an event."); repository.remove(code); }
