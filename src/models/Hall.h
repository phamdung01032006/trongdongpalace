#pragma once
#include <string>
using namespace std;

enum class HallStatus {
    ACTIVE,
    INACTIVE
};

class Hall {
private:
    int id;
    int centerId;
    string hallCode;
    string name;
    int capacity;
    HallStatus status;

public:
    Hall(int id,
        int centerId,
        const string& hallCode,
        const string& name,
        int capacity,
        HallStatus status)
        : id(id),
        centerId(centerId),
        hallCode(hallCode),
        name(name),
        capacity(capacity),
        status(status) {}
    
    // Getters
    int getId() const { return id; }
    int getCenterId() const { return centerId; }
    const string& getHallCode() const { return hallCode; }
    const string& getName() const { return name; }
    int getCapacity() const { return capacity; }
    HallStatus getStatus() const { return status; }

    // Setters
    void setCenterId(int newCenterId) { centerId = newCenterId; }
    void setName(const string& newName) { name = newName; }
    void setCapacity(int newCapacity) { capacity = newCapacity; }
    void setStatus(HallStatus newStatus) { status = newStatus; }
};