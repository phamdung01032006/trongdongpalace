#include "models/Payment.hpp"

using namespace std;

namespace tdp {

Payment::Payment(string id, string bookingId, int64_t amount, DateTime paidAt, PaymentMethod method,
                 string note, string recordedBy)
    : id_(move(id)),
      bookingId_(move(bookingId)),
      note_(move(note)),
      recordedBy_(move(recordedBy)),
      amount_(amount),
      paidAt_(paidAt),
      method_(method) {}

nlohmann::json Payment::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"amount", amount_},
            {"paidAt", dateToJson(paidAt_)},
            {"method", toString(method_)},
            {"note", note_},
            {"recordedBy", recordedBy_}};
}

Payment Payment::fromJson(const nlohmann::json& j) {
    // Tuong thich du lieu cu: chap nhan ca "bookingId" va "contractId".
    string bookingId = j.value("bookingId", "");
    if (bookingId.empty()) bookingId = j.value("contractId", "");
    return Payment(j.at("id").get<string>(), bookingId, j.value("amount", int64_t(0)),
                   dateFromJson(j.at("paidAt")), paymentMethodFromString(j.value("method", "ChuyenKhoan")),
                   j.value("note", ""), j.value("recordedBy", ""));
}

}  // namespace tdp
