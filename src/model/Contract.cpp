#include "Contract.h"
#include "Exceptions.h"
Contract::Contract(const string& id, const string& quotationId, const string& signedDate, const string& status) : id(id), quotationId(quotationId), signedDate(signedDate), status(status) {
    if (id.empty() || quotationId.empty()) throw ValidationException("Contract ID and quotation ID cannot be empty.");
}
const string& Contract::getId() const { return id; } const string& Contract::getQuotationId() const { return quotationId; } const string& Contract::getSignedDate() const { return signedDate; } const string& Contract::getStatus() const { return status; }
