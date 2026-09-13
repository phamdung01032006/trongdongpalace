#pragma once

#include <string>

using namespace std;

class User {
private:
    string id;
    string username;
    string password;
    string fullName;
    string role;

public:
    User(const string& id, const string& username, const string& password,
         const string& fullName, const string& role);

    const string& getId() const;
    const string& getUsername() const;
    const string& getPassword() const;
    const string& getFullName() const;
    const string& getRole() const;

    void setUsername(const string& username);
    void setPassword(const string& password);
    void setFullName(const string& fullName);
    void setRole(const string& role);
};
