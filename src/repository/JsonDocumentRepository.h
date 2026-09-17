#pragma once

#include "JsonStorage.h"
#include "Repository.h"

#include <utility>

// Reusable persistence foundation for domains whose model fields are defined
// in later days. Every document must contain a unique string `id` field.
class JsonDocumentRepository : public Repository<json> {
private:
    JsonStorage& storage;
    string collectionName;
    vector<json> documents;

    int findIndex(const string& id) const;
    void validateDocument(const json& document) const;

public:
    JsonDocumentRepository(JsonStorage& storage, string collectionName);

    void load() override;
    void save() override;
    void add(const json& document) override;
    void update(const json& document) override;
    void remove(const string& id) override;
    json findById(const string& id) const override;
    vector<json> getAll() const override;
};
