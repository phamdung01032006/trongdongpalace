#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Contract {
public:
    Contract() = default;
    Contract(std::string id, std::string bookingId, std::string customerId, std::string contractNo,
             DateTime signedAt, int64_t totalAmount, ContractStatus status);

    const std::string& id() const { return id_; }
    const std::string& bookingId() const { return bookingId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& contractNo() const { return contractNo_; }
    DateTime signedAt() const { return signedAt_; }
    int64_t totalAmount() const { return totalAmount_; }
    ContractStatus status() const { return status_; }
    void setStatus(ContractStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static Contract fromJson(const nlohmann::json& j);

private:
    std::string id_, bookingId_, customerId_, contractNo_;
    DateTime signedAt_;
    int64_t totalAmount_ = 0;
    ContractStatus status_ = ContractStatus::HieuLuc;
};

}  // namespace tdp
