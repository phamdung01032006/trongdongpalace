#include "JsonDocumentRepository.h"
#include "Exceptions.h"

JsonDocumentRepository::JsonDocumentRepository(JsonStorage& storage, string collectionName)
    : storage(storage), collectionName(move(collectionName)) {
    load();
}

int JsonDocumentRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < documents.size(); ++i) {
        if (documents[i].at("id").get<string>() == id) return static_cast<int>(i);
    }
    return -1;
}

void JsonDocumentRepository::validateDocument(const json& document) const {
    if (!document.is_object() || !document.contains("id") || !document.at("id").is_string() ||
        document.at("id").get<string>().empty()) {
        throw ValidationException("Repository document requires a non-empty string ID.");
    }
}

void JsonDocumentRepository::load() {
    documents = storage.load().at(collectionName).get<vector<json>>();
    for (const json& document : documents) validateDocument(document);
}

void JsonDocumentRepository::save() {
    json data = storage.load();
    data[collectionName] = documents;
    storage.save(data);
}

void JsonDocumentRepository::add(const json& document) {
    validateDocument(document);
    const string id = document.at("id").get<string>();
    if (findIndex(id) >= 0) throw DuplicateException("ID already exists: " + id);
    documents.push_back(document);
    save();
}

void JsonDocumentRepository::update(const json& document) {
    validateDocument(document);
    const string id = document.at("id").get<string>();
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Document not found: " + id);
    documents[index] = document;
    save();
}

void JsonDocumentRepository::remove(const string& id) {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Document not found: " + id);
    documents.erase(documents.begin() + index);
    save();
}

json JsonDocumentRepository::findById(const string& id) const {
    const int index = findIndex(id);
    if (index < 0) throw NotFoundException("Document not found: " + id);
    return documents[index];
}

vector<json> JsonDocumentRepository::getAll() const { return documents; }
