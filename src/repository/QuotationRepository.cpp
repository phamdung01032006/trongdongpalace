#include "QuotationRepository.h"
#include "Exceptions.h"

using namespace std;

QuotationRepository::QuotationRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Quotation QuotationRepository::fromJson(const json& item) {
    try {
        return Quotation(
            item.at("id").get<string>(),
            item.at("eventId").get<string>(),
            item.at("total").get<double>(),
            item.value("status", "")
        );
    } catch (const json::exception& e) {
        throw FileException("Invalid quotation data: " + string(e.what()));
    }
}

json QuotationRepository::toJson(const Quotation& quotation) {
    return {
        {"id", quotation.getId()},
        {"eventId", quotation.getEventId()},
        {"total", quotation.getTotal()},
        {"status", quotation.getStatus()}
    };
}

int QuotationRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < quotations.size(); ++i) {
        if (quotations[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

void QuotationRepository::load() {
    quotations.clear();

    const json data = storage.load();

    for (const auto& item : data.at("quotations"))
        quotations.push_back(fromJson(item));
}

void QuotationRepository::save() {
    json data = storage.load();

    data["quotations"] = json::array();

    for (const Quotation& quotation : quotations)
        data["quotations"].push_back(toJson(quotation));

    storage.save(data);
}

void QuotationRepository::add(const Quotation& quotation) {
    if (findIndex(quotation.getId()) >= 0)
        throw DuplicateException(
            "Quotation ID already exists: " + quotation.getId()
        );

    quotations.push_back(quotation);
    save();
}

void QuotationRepository::update(const Quotation& quotation) {
    const int index = findIndex(quotation.getId());

    if (index < 0)
        throw NotFoundException(
            "Quotation not found: " + quotation.getId()
        );

    quotations[index] = quotation;
    save();
}

void QuotationRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Quotation not found: " + id
        );

    quotations.erase(quotations.begin() + index);
    save();
}

Quotation QuotationRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Quotation not found: " + id
        );

    return quotations[index];
}

vector<Quotation> QuotationRepository::getAll() const {
    return quotations;
}