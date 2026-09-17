#pragma once

#include "UserRepository.h"

class AuthService {
private:
    UserRepository& userRepository;

public:
    explicit AuthService(UserRepository& userRepository);

    void seedDefaultUsers();
    User login(const string& username, const string& password) const;
    bool hasPermission(const User& user, const string& module, const string& action) const;
    void requirePermission(const User& user, const string& module, const string& action) const;
};
