#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Payment {
public:
    Payment() = default;
    Payment(string id, string bookingId, int64_t amount, DateTime paidAt, PaymentMethod method,
            string note, string recordedBy);

    const string& id() const { return id_; }
    const string& bookingId() const { return bookingId_; }
    int64_t amount() const { return amount_; }
    DateTime paidAt() const { return paidAt_; }
    PaymentMethod method() const { return method_; }
    const string& note() const { return note_; }
    const string& recordedBy() const { return recordedBy_; }

    nlohmann::json toJson() const;
    static Payment fromJson(const nlohmann::json& j);

private:
    string id_, bookingId_, note_, recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
};

}  // namespace tdp
