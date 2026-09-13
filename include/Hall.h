#pragma once

#include <string>

using namespace std;

class Hall {
private:
    string id;
    string centerId;
    string name;
    int capacity;
    string status;

public:
    Hall(const string& id, const string& centerId, const string& name, int capacity,
         const string& status = "ACTIVE");

    const string& getId() const;
    const string& getCenterId() const;
    const string& getName() const;
    int getCapacity() const;
    const string& getStatus() const;
    bool isActive() const;

    void setName(const string& name);
    void setCapacity(int capacity);
    void setStatus(const string& status);
};
