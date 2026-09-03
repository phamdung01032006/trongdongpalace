#pragma once
#include <string>
using namespace std;

enum class ServiceStatus {
    ACTIVE,
    INACTIVE
};

class ServiceItem {
private:
    int id;
    string serviceCode;
    string name;
    string description;
    string unit;
    double unitPrice;
    ServiceStatus status;

public:
    ServiceItem(int id,
                const string& serviceCode,
                const string& name,
                const string& description,
                const string& unit,
                double unitPrice,
                ServiceStatus status)
                : id(id),
                serviceCode(serviceCode),
                name(name),
                description(description),
                unit(unit),
                unitPrice(unitPrice),
                status(status) {}
    
    // Getters
    int getId() const { return id; }
    const string& getServiceCode() const { return serviceCode; }
    const string& getName() const { return name; }
    const string& getDescription() const { return description; }
    const string& getUnit() const { return unit; }
    double getUnitPrice() const { return unitPrice; }
    ServiceStatus getStatus() const { return status; }

    // Setters
    void setName(const string& newName) { name = newName; }
    void setDescription(const string& newDescription) { description = newDescription; }
    void setUnit(const string& newUnit) { unit = newUnit; }
    void setUnitPrice(double newUnitPrice) { unitPrice = newUnitPrice; }
    void setStatus(ServiceStatus newStatus) { status = newStatus; }
};