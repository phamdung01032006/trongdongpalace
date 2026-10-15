#ifndef TDP_MODELS_PAYMENT_CPP
#define TDP_MODELS_PAYMENT_CPP

#include "models/Enums.cpp"
#include "utils/helpers.cpp"

#include <cstdint>
#include "utils/text_fields.cpp"
#include <string>

using namespace std;

// Khoan thu (dac ta B.3 + Form 3): tu dong tao sau khi Booking hoan thanh,
// gia tri ban dau de trong (amount = 0), ghi chu bat buoc.
class Payment {
public:
    Payment() = default;

    Payment(const string& id, const string& bookingId, PaymentType type, int64_t amount, DateTime paidAt,
             PaymentMethod method, PaymentStatus status, const string& note, const string& recordedBy)
        : id_(id),
          bookingId_(bookingId),
          note_(note),
          recordedBy_(recordedBy),
          amount_(amount),
          paidAt_(paidAt),
          type_(type),
          method_(method),
          status_(status) {}

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
    void setNote(const string& value) { note_ = value; }

    vector<string> toFields() const;
    static Payment fromFields(const vector<string>& f);

private:
    string id_, bookingId_, note_, recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentType type_ = PaymentType::DatCoc1;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
    PaymentStatus status_ = PaymentStatus::ChuaThanhToan;
};

vector<string> Payment::toFields() const {
    vector<string> f = {id_, bookingId_, toString(type_), to_string(amount_), paidAt_.toString(), toString(method_), toString(status_), note_, recordedBy_};
    return f;
}

Payment Payment::fromFields(const vector<string>& f) {
    requireFieldCount(f, 9);
    Payment item;
    item.id_ = f.at(0);
    item.bookingId_ = f.at(1);
    item.type_ = paymentTypeFromString(f.at(2));
    if (toString(item.type_) != f.at(2)) throw ValidationException("Trang thai/loai khong hop le");
    item.amount_ = textMoney(f.at(3));
    item.paidAt_ = textDate(f.at(4));
    item.method_ = paymentMethodFromString(f.at(5));
    if (toString(item.method_) != f.at(5)) throw ValidationException("Trang thai/loai khong hop le");
    item.status_ = paymentStatusFromString(f.at(6));
    if (toString(item.status_) != f.at(6)) throw ValidationException("Trang thai/loai khong hop le");
    item.note_ = f.at(7);
    item.recordedBy_ = f.at(8);
    return item;
}

#endif
