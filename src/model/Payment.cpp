#include "Payment.h"
#include "Exceptions.h"
Payment::Payment(const string& id, const string& contractId, double amount, const string& paymentDate, const string& status) : id(id), contractId(contractId), paymentDate(paymentDate), status(status), amount(amount) {
    if (id.empty() || contractId.empty()) throw ValidationException("Payment ID and contract ID cannot be empty."); if (amount < 0) throw ValidationException("Payment amount cannot be negative.");
}
const string& Payment::getId() const { return id; } const string& Payment::getContractId() const { return contractId; } double Payment::getAmount() const { return amount; } const string& Payment::getPaymentDate() const { return paymentDate; } const string& Payment::getStatus() const { return status; }
