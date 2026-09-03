#pragma once
#include <string>
using namespace std;

enum class UserRole {
    ADMIN,
    SALES,
    ACCOUNTANT,
    COORDINATOR,
    OPERATIONS,
    MANAGEMENT
};

enum class UserStatus {
    ACTIVE,
    INACTIVE
};

class User {
private:
    int id;
    string username;
    string passwordHash;
    UserRole role;
    UserStatus status;

public:
    User(int id,
        const string& username,
        const string& passwordHash,
        UserRole role,
        UserStatus status)
        : id(id),
        username(username),
        passwordHash(passwordHash),
        role(role),
        status(status) {}

    // Getters
    int getId() const {return id; }
    const string& getUsername() const { return username; }
    const string& getPasswordHash() const { return passwordHash; }
    UserRole getRole() const { return role; }
    UserStatus getStatus() const { return status; }

    // Setters
    void setUsername(const string& newUsername) { username = newUsername; }
    void setPasswordHash(const string& newPasswordHash) { passwordHash = newPasswordHash; }
    void setRole(UserRole newRole) { role = newRole; }
    void setStatus(UserStatus newStatus) { status = newStatus; }
};