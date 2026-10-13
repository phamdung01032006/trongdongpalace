#include "models/Payment.hpp"

using namespace std;

namespace tdp {

Payment::Payment(string id, string bookingId, PaymentType type, int64_t amount, DateTime paidAt,
                 PaymentMethod method, PaymentStatus status, string note, string recordedBy)
    : id_(move(id)),
      bookingId_(move(bookingId)),
      note_(move(note)),
      recordedBy_(move(recordedBy)),
      amount_(amount),
      paidAt_(paidAt),
      type_(type),
      method_(method),
      status_(status) {}

nlohmann::json Payment::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"type", toString(type_)},
            {"amount", amount_},
            {"paidAt", dateToJson(paidAt_)},
            {"method", toString(method_)},
            {"status", toString(status_)},
            {"note", note_},
            {"recordedBy", recordedBy_}};
}

Payment Payment::fromJson(const nlohmann::json& j) {
    // Tuong thich du lieu cu: chap nhan ca "bookingId" va "contractId".
    string bookingId = j.value("bookingId", "");
    if (bookingId.empty()) bookingId = j.value("contractId", "");
    PaymentStatus status = PaymentStatus::ChuaThanhToan;
    if (j.contains("status")) {
        status = paymentStatusFromString(j.value("status", "Chưa thanh toán"));
    } else if (j.value("amount", int64_t(0)) > 0) {
        status = PaymentStatus::DaThanhToan;
    }
    return Payment(j.at("id").get<string>(), bookingId,
                   paymentTypeFromString(j.value("type", "Đặt cọc lần 1")),
                   j.value("amount", int64_t(0)), dateFromJson(j.at("paidAt")),
                   paymentMethodFromString(j.value("method", "Chuyển khoản")), status,
                   j.value("note", ""), j.value("recordedBy", ""));
}

}  // namespace tdp
