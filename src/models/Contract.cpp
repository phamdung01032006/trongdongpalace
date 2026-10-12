#include "models/Contract.hpp"

namespace tdp {

Contract::Contract(std::string id, std::string bookingId, std::string customerId, std::string contractNo,
                   DateTime signedAt, int64_t totalAmount, ContractStatus status)
    : id_(std::move(id)),
      bookingId_(std::move(bookingId)),
      customerId_(std::move(customerId)),
      contractNo_(std::move(contractNo)),
      signedAt_(signedAt),
      totalAmount_(totalAmount),
      status_(status) {}

nlohmann::json Contract::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"customerId", customerId_},
            {"contractNo", contractNo_},
            {"signedAt", dateToJson(signedAt_)},
            {"totalAmount", totalAmount_},
            {"status", toString(status_)}};
}

Contract Contract::fromJson(const nlohmann::json& j) {
    return Contract(j.at("id").get<std::string>(), j.at("bookingId").get<std::string>(),
                    j.at("customerId").get<std::string>(), j.value("contractNo", ""), dateFromJson(j.at("signedAt")),
                    j.value("totalAmount", 0), contractStatusFromString(j.value("status", "HieuLuc")));
}

}  // namespace tdp
