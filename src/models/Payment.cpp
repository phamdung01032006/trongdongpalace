#include "models/Payment.hpp"

using namespace std;

namespace tdp {

Payment::Payment(string id, string contractId, int64_t amount, DateTime paidAt, PaymentMethod method,
                 string note, string recordedBy)
    : id_(move(id)),
      contractId_(move(contractId)),
      note_(move(note)),
      recordedBy_(move(recordedBy)),
      amount_(amount),
      paidAt_(paidAt),
      method_(method) {}

nlohmann::json Payment::toJson() const {
    return {{"id", id_},
            {"contractId", contractId_},
            {"amount", amount_},
            {"paidAt", dateToJson(paidAt_)},
            {"method", toString(method_)},
            {"note", note_},
            {"recordedBy", recordedBy_}};
}

Payment Payment::fromJson(const nlohmann::json& j) {
    return Payment(j.at("id").get<string>(), j.at("contractId").get<string>(), j.at("amount").get<int64_t>(),
                   dateFromJson(j.at("paidAt")), paymentMethodFromString(j.value("method", "ChuyenKhoan")),
                   j.value("note", ""), j.value("recordedBy", ""));
}

}  // namespace tdp
