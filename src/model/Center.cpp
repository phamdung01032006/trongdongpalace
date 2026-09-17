#include "Center.h"
#include "Exceptions.h"

using namespace std;

Center::Center(const string& id, const string& name, const string& address) : id(id), address(address) {
    if (id.empty()) throw ValidationException("Center ID cannot be empty.");
    setName(name);
}

const string& Center::getId() const { return id; }
const string& Center::getName() const { return name; }
const string& Center::getAddress() const { return address; }

void Center::setName(const string& value) {
    if (value.empty()) throw ValidationException("Center name cannot be empty.");
    name = value;
}
void Center::setAddress(const string& value) { address = value; }
