#pragma once

#include <string>

using namespace std;

class Center {
private:
    string id;
    string name;
    string address;

public:
    Center(const string& id, const string& name, const string& address = "");

    const string& getId() const;
    const string& getName() const;
    const string& getAddress() const;

    void setName(const string& name);
    void setAddress(const string& address);
};
