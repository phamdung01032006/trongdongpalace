#pragma once
#include <string>
using namespace std;

enum class ContractStatus {
    DRAFT,
    ACTIVE,
    COMPLETED,
    CANCELLED
};

class Contract {
private:
    int id;
    string contractCode;
    int quotationId;
    int customerId;
    int eventId;
    double totalAmount;
    ContractStatus status;
    string contractDate;

public:
    Contract(int id,
            const string& contractCode,
            int quotationId,
            int customerId,
            int eventId,
            double totalAmount,
            ContractStatus status,
            const string& contractData)
            : id(id),
            contractCode(contractCode),
            quotationId(quotationId),
            customerId(customerId),
            eventId(eventId),
            totalAmount(totalAmount),
            status(status),
            contractDate(contractDate) {}
    
    // Getters
    int getId() const { return id; }
    const string& getContractCode() const { return contractCode; }
    int getQuotationId() const { return quotationId; }
    int getCustomerId() const { return customerId; }
    int getEventId() const { return eventId; }
    double getTotalAmount() const { return totalAmount; }
    ContractStatus getStatus() const { return status; }
    const string& getContractDate() const { return contractDate; }

    // Setters
    void setQuotationId(int newQuotationId) { quotationId = newQuotationId; }
    void setCustomerId(int newCustomerId) { customerId = newCustomerId; }
    void setEventId(int newEventId) { eventId = newEventId; }
    void setTotalAmount(double newTotalAmount) { totalAmount = newTotalAmount; }
    void setStatus(ContractStatus newStatus) { status = newStatus; }
    void setContractDate(const string& newContractDate) { contractDate = newContractDate; }
};