#include "Package.h"
#include "Exceptions.h"

using namespace std;

namespace {
bool validStatus(const string& value) { return value == "ACTIVE" || value == "INACTIVE"; }
}

Package::Package(const string& id, const string& name, const string& description,
                 double price, int guestCapacity, const string& status) : id(id) {
    if (id.empty()) throw ValidationException("Package ID cannot be empty.");
    setName(name);
    setDescription(description);
    setPrice(price);
    setGuestCapacity(guestCapacity);
    setStatus(status);
}

const string& Package::getId() const { return id; }
const string& Package::getName() const { return name; }
const string& Package::getDescription() const { return description; }
double Package::getPrice() const { return price; }
int Package::getGuestCapacity() const { return guestCapacity; }
const string& Package::getStatus() const { return status; }

void Package::setName(const string& value) {
    if (value.empty()) throw ValidationException("Package name cannot be empty.");
    name = value;
}
void Package::setDescription(const string& value) { description = value; }
void Package::setPrice(double value) {
    if (value < 0) throw ValidationException("Package price cannot be negative.");
    price = value;
}
void Package::setGuestCapacity(int value) {
    if (value <= 0) throw ValidationException("Guest capacity must be greater than 0.");
    guestCapacity = value;
}
void Package::setStatus(const string& value) {
    if (!validStatus(value)) throw ValidationException("Package status must be ACTIVE or INACTIVE.");
    status = value;
}
