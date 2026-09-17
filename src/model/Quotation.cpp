#include "Quotation.h"
#include "Exceptions.h"
Quotation::Quotation(const string& id, const string& eventId, double total, const string& status) : id(id), eventId(eventId), status(status), total(total) {
    if (id.empty() || eventId.empty()) throw ValidationException("Quotation ID and event ID cannot be empty."); if (total < 0) throw ValidationException("Quotation total cannot be negative.");
}
const string& Quotation::getId() const { return id; } const string& Quotation::getEventId() const { return eventId; } double Quotation::getTotal() const { return total; } const string& Quotation::getStatus() const { return status; }
