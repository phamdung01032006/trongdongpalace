#include "models/Contract.hpp"

using namespace std;

namespace tdp {

Contract::Contract(string id, string bookingId, string customerId, string contractNo,
                   DateTime signedAt, int64_t totalAmount, ContractStatus status)
    : id_(move(id)),
      bookingId_(move(bookingId)),
      customerId_(move(customerId)),
      contractNo_(move(contractNo)),
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
    return Contract(j.at("id").get<string>(), j.at("bookingId").get<string>(),
                    j.at("customerId").get<string>(), j.value("contractNo", ""), dateFromJson(j.at("signedAt")),
                    j.value("totalAmount", 0), contractStatusFromString(j.value("status", "HieuLuc")));
}

}  // namespace tdp
