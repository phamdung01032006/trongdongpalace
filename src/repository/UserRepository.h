#pragma once

#include "JsonStorage.h"
#include "Repository.h"
#include "User.h"

class UserRepository : public Repository<User> {
private:
    JsonStorage& storage;
    vector<User> users;

    static User fromJson(const json& item);
    static json toJson(const User& user);
    int findIndex(const string& id) const;

public:
    explicit UserRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const User& user) override;
    void update(const User& user) override;
    void remove(const string& id) override;
    User findById(const string& id) const override;
    User findByUsername(const string& username) const;
    vector<User> getAll() const override;
};
