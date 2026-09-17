#pragma once
#include <string>
using namespace std;
class Contract {
    string id, quotationId, signedDate, status;
public:
    Contract(const string& id, const string& quotationId, const string& signedDate, const string& status = "DRAFT");
    const string& getId() const; const string& getQuotationId() const; const string& getSignedDate() const; const string& getStatus() const;
};
