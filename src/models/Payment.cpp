#include "models/Payment.hpp"

namespace tdp {

Payment::Payment(std::string id, std::string contractId, int64_t amount, DateTime paidAt, PaymentMethod method,
                 std::string note, std::string recordedBy)
    : id_(std::move(id)),
      contractId_(std::move(contractId)),
      note_(std::move(note)),
      recordedBy_(std::move(recordedBy)),
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
    return Payment(j.at("id").get<std::string>(), j.at("contractId").get<std::string>(), j.at("amount").get<int64_t>(),
                   dateFromJson(j.at("paidAt")), paymentMethodFromString(j.value("method", "ChuyenKhoan")),
                   j.value("note", ""), j.value("recordedBy", ""));
}

}  // namespace tdp
