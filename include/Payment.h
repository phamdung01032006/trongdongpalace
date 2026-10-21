#pragma once

// Mot khoan thu cua booking (dat coc lan 1, dat coc lan 2, tat toan).
// Khi tao booking, he thong tu tao san mot khoan "Dat coc lan 1" voi amount = 0.
// Nhan vien kinh doanh chi viec dien so tien va ghi chu.

#include "DateTime.h"
#include "Enums.h"

#include <cstdint>
#include <string>

using namespace std;

class Payment {
public:
    Payment(string id = "", string bookingId = "", PaymentType type = PaymentType::DatCoc1,
            int64_t amount = 0, DateTime paidAt = DateTime{},
            PaymentMethod method = PaymentMethod::ChuyenKhoan,
            PaymentStatus status = PaymentStatus::ChuaThanhToan, string note = "",
            string recordedBy = "");

    const string& get_id() const { return id_; }
    const string& get_bookingId() const { return bookingId_; }
    PaymentType get_type() const { return type_; }
    int64_t get_amount() const { return amount_; }
    const DateTime& get_paidAt() const { return paidAt_; }
    PaymentMethod get_method() const { return method_; }
    PaymentStatus get_status() const { return status_; }
    const string& get_note() const { return note_; }
    const string& get_recordedBy() const { return recordedBy_; }

    void set_type(PaymentType value) { type_ = value; }
    void set_amount(int64_t value) { amount_ = value; }
    void set_paidAt(const DateTime& value) { paidAt_ = value; }
    void set_method(PaymentMethod value) { method_ = value; }
    void set_status(PaymentStatus value) { status_ = value; }
    void set_note(const string& value) { note_ = value; }
    void set_recordedBy(const string& value) { recordedBy_ = value; }

    string toText() const;
    static Payment fromText(const string& line);

private:
    string id_;
    string bookingId_;
    string note_;
    string recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentType type_ = PaymentType::DatCoc1;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
    PaymentStatus status_ = PaymentStatus::ChuaThanhToan;
};
