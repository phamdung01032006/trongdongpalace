#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Contract {
public:
    Contract() = default;
    Contract(string id, string bookingId, string customerId, string contractNo,
             DateTime signedAt, int64_t totalAmount, ContractStatus status);

    const string& id() const { return id_; }
    const string& bookingId() const { return bookingId_; }
    const string& customerId() const { return customerId_; }
    const string& contractNo() const { return contractNo_; }
    DateTime signedAt() const { return signedAt_; }
    int64_t totalAmount() const { return totalAmount_; }
    ContractStatus status() const { return status_; }
    void setStatus(ContractStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static Contract fromJson(const nlohmann::json& j);

private:
    string id_, bookingId_, customerId_, contractNo_;
    DateTime signedAt_;
    int64_t totalAmount_ = 0;
    ContractStatus status_ = ContractStatus::HieuLuc;
};

}  // namespace tdp
