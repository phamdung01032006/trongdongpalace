#include "PaymentRepository.h"
#include "Exceptions.h"

using namespace std;

PaymentRepository::PaymentRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Payment PaymentRepository::fromJson(const json& item) {
    try {
        return Payment(
            item.at("id").get<string>(),
            item.at("contractId").get<string>(),
            item.at("amount").get<double>(),
            item.at("paymentDate").get<string>(),
            item.value("status", "")
        );
    } catch (const json::exception& e) {
        throw FileException(
            "Invalid payment data: " + string(e.what())
        );
    }
}

json PaymentRepository::toJson(const Payment& payment) {
    return {
        {"id", payment.getId()},
        {"contractId", payment.getContractId()},
        {"amount", payment.getAmount()},
        {"paymentDate", payment.getPaymentDate()},
        {"status", payment.getStatus()}
    };
}

int PaymentRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < payments.size(); ++i) {
        if (payments[i].getId() == id)
            return static_cast<int>(i);
    }

    return -1;
}

void PaymentRepository::load() {
    payments.clear();

    const json data = storage.load();

    for (const auto& item : data.at("payments"))
        payments.push_back(fromJson(item));
}

void PaymentRepository::save() {
    json data = storage.load();

    data["payments"] = json::array();

    for (const Payment& payment : payments)
        data["payments"].push_back(toJson(payment));

    storage.save(data);
}

void PaymentRepository::add(const Payment& payment) {
    if (findIndex(payment.getId()) >= 0)
        throw DuplicateException(
            "Payment ID already exists: " + payment.getId()
        );

    payments.push_back(payment);
    save();
}

void PaymentRepository::update(const Payment& payment) {
    const int index = findIndex(payment.getId());

    if (index < 0)
        throw NotFoundException(
            "Payment not found: " + payment.getId()
        );

    payments[index] = payment;
    save();
}

void PaymentRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Payment not found: " + id
        );

    payments.erase(payments.begin() + index);
    save();
}

Payment PaymentRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Payment not found: " + id
        );

    return payments[index];
}

vector<Payment> PaymentRepository::getAll() const {
    return payments;
}