#include "Service.h"
#include "Exceptions.h"

using namespace std;

namespace {
bool validStatus(const string& value) { return value == "ACTIVE" || value == "INACTIVE"; }
}

Service::Service(const string& id, const string& name, const string& unit,
                 double unitPrice, const string& status) : id(id) {
    if (id.empty()) throw ValidationException("Service ID cannot be empty.");
    setName(name);
    setUnit(unit);
    setUnitPrice(unitPrice);
    setStatus(status);
}

const string& Service::getId() const { return id; }
const string& Service::getName() const { return name; }
const string& Service::getUnit() const { return unit; }
double Service::getUnitPrice() const { return unitPrice; }
const string& Service::getStatus() const { return status; }

void Service::setName(const string& value) {
    if (value.empty()) throw ValidationException("Service name cannot be empty.");
    name = value;
}
void Service::setUnit(const string& value) {
    if (value.empty()) throw ValidationException("Service unit cannot be empty.");
    unit = value;
}
void Service::setUnitPrice(double value) {
    if (value < 0) throw ValidationException("Service unit price cannot be negative.");
    unitPrice = value;
}
void Service::setStatus(const string& value) {
    if (!validStatus(value)) throw ValidationException("Service status must be ACTIVE or INACTIVE.");
    status = value;
}
