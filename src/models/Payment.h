#pragma once
#include <string>
using namespace std;

enum class PaymentMethodType {
    CASH,
    BANK_TRANSFER,
    CARD
};

class Payment {
private:
    int id;
    string paymentCode;
    int contractId;
    double amount;
    string paymentDate;
    PaymentMethodType method;
    string note;

public:
    Payment(int id,
            const string& paymentCode,
            int contractId,
            double amount,
            const string& paymentDate,
            PaymentMethodType method,
            const string& note)
            : id(id),
            paymentCode(paymentCode),
            contractId(contractId),
            amount(amount),
            paymentDate(paymentDate),
            method(method),
            note(note) {}
    
    // Getters
    int getId() const { return id; }
    const string& getPaymentCode() const { return paymentCode; }
    int getContractId() const { return contractId; }
    double getAmount() const { return amount; }
    const string& getPaymentDate() const { return paymentDate; }
    PaymentMethodType getMethod() const { return method; }
    const string& getNote() const { return note; }

    // Setters
    void setContractId(int newContractId) { contractId = newContractId; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setPaymentDate(const string& newPaymentDate) { paymentDate = newPaymentDate; }
    void setMethod(PaymentMethodType newMethod) { method = newMethod; }
    void setNote(const string& newNote) { note = newNote; }
};