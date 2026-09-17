#include "ContractRepository.h"
#include "Exceptions.h"

using namespace std;

ContractRepository::ContractRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Contract ContractRepository::fromJson(const json& item) {
    try {
        return Contract(
            item.at("id").get<string>(),
            item.at("quotationId").get<string>(),
            item.at("signedDate").get<string>(),
            item.value("status", "")
        );
    } catch (const json::exception& e) {
        throw FileException("Invalid contract data: " + string(e.what()));
    }
}

json ContractRepository::toJson(const Contract& contract) {
    return {
        {"id", contract.getId()},
        {"quotationId", contract.getQuotationId()},
        {"signedDate", contract.getSignedDate()},
        {"status", contract.getStatus()}
    };
}

int ContractRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < contracts.size(); ++i) {
        if (contracts[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

void ContractRepository::load() {
    contracts.clear();

    const json data = storage.load();

    for (const auto& item : data.at("contracts"))
        contracts.push_back(fromJson(item));
}

void ContractRepository::save() {
    json data = storage.load();

    data["contracts"] = json::array();

    for (const Contract& contract : contracts)
        data["contracts"].push_back(toJson(contract));

    storage.save(data);
}

void ContractRepository::add(const Contract& contract) {
    if (findIndex(contract.getId()) >= 0)
        throw DuplicateException(
            "Contract ID already exists: " + contract.getId()
        );

    contracts.push_back(contract);
    save();
}

void ContractRepository::update(const Contract& contract) {
    const int index = findIndex(contract.getId());

    if (index < 0)
        throw NotFoundException(
            "Contract not found: " + contract.getId()
        );

    contracts[index] = contract;
    save();
}

void ContractRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Contract not found: " + id
        );

    contracts.erase(contracts.begin() + index);
    save();
}

Contract ContractRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Contract not found: " + id
        );

    return contracts[index];
}

vector<Contract> ContractRepository::getAll() const {
    return contracts;
}