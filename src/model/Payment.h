#pragma once
#include <string>
using namespace std;
class Payment {
    string id, contractId, paymentDate, status; double amount;
public:
    Payment(const string& id, const string& contractId, double amount, const string& paymentDate, const string& status = "PENDING");
    const string& getId() const; const string& getContractId() const; double getAmount() const; const string& getPaymentDate() const; const string& getStatus() const;
};
