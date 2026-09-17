#pragma once

#include "Exceptions.h"
#include "JsonStorage.h"
#include "Repository.h"

// Shared implementation for repositories backed by one array in data.json.
template <typename T>
class TypedJsonRepository : public Repository<T> {
private:
    JsonStorage& storage;
    string collectionName;
    string entityName;
    vector<T> items;

    int findIndex(const string& id) const {
        for (size_t i = 0; i < items.size(); ++i) {
            if (getId(items[i]) == id) return static_cast<int>(i);
        }
        return -1;
    }

protected:
    TypedJsonRepository(JsonStorage& storage, string collectionName, string entityName)
        : storage(storage), collectionName(move(collectionName)), entityName(move(entityName)) {}

    virtual string getId(const T& item) const = 0;
    virtual T fromJson(const json& item) const = 0;
    virtual json toJson(const T& item) const = 0;

public:
    void load() override {
        items.clear();
        for (const json& item : storage.load().at(collectionName)) items.push_back(fromJson(item));
    }

    void save() override {
        json data = storage.load();
        data[collectionName] = json::array();
        for (const T& item : items) data[collectionName].push_back(toJson(item));
        storage.save(data);
    }

    void add(const T& item) override {
        const string id = getId(item);
        if (findIndex(id) >= 0) throw DuplicateException(entityName + " ID already exists: " + id);
        items.push_back(item);
        save();
    }

    void update(const T& item) override {
        const string id = getId(item);
        const int index = findIndex(id);
        if (index < 0) throw NotFoundException(entityName + " not found: " + id);
        items[index] = item;
        save();
    }

    void remove(const string& id) override {
        const int index = findIndex(id);
        if (index < 0) throw NotFoundException(entityName + " not found: " + id);
        items.erase(items.begin() + index);
        save();
    }

    T findById(const string& id) const override {
        const int index = findIndex(id);
        if (index < 0) throw NotFoundException(entityName + " not found: " + id);
        return items[index];
    }

    vector<T> getAll() const override { return items; }
};
