#include "UserRepository.h"
#include "Exceptions.h"

using namespace std;

UserRepository::UserRepository(JsonStorage& storage) : storage(storage) { load(); }

User UserRepository::fromJson(const json& item) {
    try {
        return User(item.at("id").get<string>(), item.at("username").get<string>(),
                    item.at("password").get<string>(), item.at("fullName").get<string>(),
                    item.at("role").get<string>());
    } catch (const json::exception& e) {
        throw FileException("Invalid user data: " + string(e.what()));
    }
}

json UserRepository::toJson(const User& user) {
    return {{"id", user.getId()}, {"username", user.getUsername()}, {"password", user.getPassword()},
            {"fullName", user.getFullName()}, {"role", user.getRole()}};
}

int UserRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

void UserRepository::load() {
    users.clear();
    const json data = storage.load();
    for (const auto& item : data.at("users")) users.push_back(fromJson(item));
}

void UserRepository::save() {
    json data = storage.load();
    data["users"] = json::array();
    for (const User& user : users) data["users"].push_back(toJson(user));
    storage.save(data);
}

void UserRepository::add(const User& user) {
    if (findIndex(user.getId()) >= 0) throw DuplicateException("User ID already exists: " + user.getId());
    for (const User& existing : users) {
        if (existing.getUsername() == user.getUsername()) {
            throw DuplicateException("Username already exists: " + user.getUsername());
        }
    }
    users.push_back(user);
    save();
}

void UserRepository::update(const User& user) {
    const int index = findIndex(user.getId());
    if (index < 0) throw NotFoundException("User not found: " + user.getId());
    for (size_t i = 0; i < users.size(); ++i) {
        if (static_cast<int>(i) != index && users[i].getUsername() == user.getUsername()) {
            throw DuplicateException("Username already exists: " + user.getUsername());
        }
    }
    users[index] = user;
    save();
}

void UserRepository::remove(const string& id) {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("User not found: " + id);
    users.erase(users.begin() + index);
    save();
}

User UserRepository::findById(const string& id) const {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("User not found: " + id);
    return users[index];
}

User UserRepository::findByUsername(const string& username) const {
    for (const User& user : users) {
        if (user.getUsername() == username) return user;
    }
    throw NotFoundException("User not found: " + username);
}

vector<User> UserRepository::getAll() const { return users; }
