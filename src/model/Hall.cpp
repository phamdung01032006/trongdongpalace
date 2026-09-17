#include "Hall.h"
#include "Exceptions.h"

using namespace std;

namespace {
bool validStatus(const string& value) { return value == "ACTIVE" || value == "INACTIVE"; }
}

Hall::Hall(const string& id, const string& centerId, const string& name, int capacity,
           const string& status) : id(id), centerId(centerId) {
    if (id.empty()) throw ValidationException("Hall ID cannot be empty.");
    if (centerId.empty()) throw ValidationException("Center ID cannot be empty.");
    setName(name);
    setCapacity(capacity);
    setStatus(status);
}

const string& Hall::getId() const { return id; }
const string& Hall::getCenterId() const { return centerId; }
const string& Hall::getName() const { return name; }
int Hall::getCapacity() const { return capacity; }
const string& Hall::getStatus() const { return status; }
bool Hall::isActive() const { return status == "ACTIVE"; }

void Hall::setName(const string& value) {
    if (value.empty()) throw ValidationException("Hall name cannot be empty.");
    name = value;
}
void Hall::setCapacity(int value) {
    if (value <= 0) throw ValidationException("Hall capacity must be greater than 0.");
    capacity = value;
}
void Hall::setStatus(const string& value) {
    if (!validStatus(value)) throw ValidationException("Hall status must be ACTIVE or INACTIVE.");
    status = value;
}
