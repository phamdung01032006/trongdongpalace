#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Payment {
public:
    Payment() = default;
    Payment(std::string id, std::string contractId, int64_t amount, DateTime paidAt, PaymentMethod method,
            std::string note, std::string recordedBy);

    const std::string& id() const { return id_; }
    const std::string& contractId() const { return contractId_; }
    int64_t amount() const { return amount_; }
    DateTime paidAt() const { return paidAt_; }
    PaymentMethod method() const { return method_; }
    const std::string& note() const { return note_; }
    const std::string& recordedBy() const { return recordedBy_; }

    nlohmann::json toJson() const;
    static Payment fromJson(const nlohmann::json& j);

private:
    std::string id_, contractId_, note_, recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
};

}  // namespace tdp
