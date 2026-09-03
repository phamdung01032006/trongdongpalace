#pragma once
#include <string>
using namespace std;

enum class PackageStatus {
    ACTIVE,
    INACTIVE
};

class Package {
private:
    int id;
    string packageCode;
    string name;
    string description;
    double basePrice;
    PackageStatus status;

public:
    Package(int id,
            const string& packageCode,
            const string& name,
            const string& description,
            double basePrice,
            PackageStatus status)
            : id(id),
            packageCode(packageCode),
            name(name),
            description(description),
            basePrice(basePrice),
            status(status) {}
    
    // Getters
    int getId() const { return id; }
    const string& getPackageCode() const { return packageCode; }
    const string& getName() const { return name; }
    const string& getDescription() const { return description; }
    double getBasePrice() const { return basePrice; }
    PackageStatus getStatus() const { return status; }

    // Setters
    void setName(const string& newName) { name = newName; }
    void setDescription(const string& newDescription) { description = newDescription; }
    void setBasePrice(double newBasePrice) { basePrice = newBasePrice; }
    void setStatus(PackageStatus newStatus) { status = newStatus; }
};