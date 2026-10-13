#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

// Khoan thu (dac ta B.3 + Form 3): tu dong tao sau khi Booking hoan thanh,
// gia tri ban dau de trong (amount = 0), ghi chu bat buoc.
class Payment {
public:
    Payment() = default;
    Payment(string id, string bookingId, PaymentType type, int64_t amount, DateTime paidAt,
            PaymentMethod method, PaymentStatus status, string note, string recordedBy);

    const string& id() const { return id_; }
    const string& bookingId() const { return bookingId_; }
    PaymentType type() const { return type_; }
    // amount = 0 nghia la chua nhap gia tri (Form 3).
    int64_t amount() const { return amount_; }
    DateTime paidAt() const { return paidAt_; }
    PaymentMethod method() const { return method_; }
    PaymentStatus status() const { return status_; }
    const string& note() const { return note_; }
    const string& recordedBy() const { return recordedBy_; }

    void setType(PaymentType v) { type_ = v; }
    void setAmount(int64_t v) { amount_ = v; }
    void setPaidAt(DateTime v) { paidAt_ = v; }
    void setMethod(PaymentMethod v) { method_ = v; }
    void setStatus(PaymentStatus v) { status_ = v; }
    void setNote(string v) { note_ = move(v); }

    nlohmann::json toJson() const;
    static Payment fromJson(const nlohmann::json& j);

private:
    string id_, bookingId_, note_, recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentType type_ = PaymentType::DatCoc1;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
    PaymentStatus status_ = PaymentStatus::ChuaThanhToan;
};

}  // namespace tdp
