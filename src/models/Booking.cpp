#include "models/Booking.hpp"

using namespace std;

namespace tdp {

Booking::Booking(string id, string requestId, string customerId, string hallId, DateTime start,
                 DateTime end, int guestCount, int64_t estimatedTotal, BookingStatus status)
    : id_(move(id)),
      requestId_(move(requestId)),
      customerId_(move(customerId)),
      hallId_(move(hallId)),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      estimatedTotal_(estimatedTotal),
      status_(status) {}

void Booking::setDiscount(int percent, int64_t amount, bool needsApproval) {
    discountPercent_ = percent;
    discountAmount_ = amount;
    discountNeedsApproval_ = needsApproval;
    if (!needsApproval) discountApproved_ = true;
}

nlohmann::json Booking::toJson() const {
    return {{"id", id_},
            {"requestId", requestId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"estimatedTotal", estimatedTotal_},
            {"discountPercent", discountPercent_},
            {"discountAmount", discountAmount_},
            {"discountNeedsApproval", discountNeedsApproval_},
            {"discountApproved", discountApproved_},
            {"status", toString(status_)},
            {"createdAt", dateToJson(createdAt_)},
            {"holdUntil", dateToJson(holdUntil_)},
            {"salesUsername", salesUsername_},
            {"cancelReason", cancelReason_}};
}

Booking Booking::fromJson(const nlohmann::json& j) {
    Booking b(j.at("id").get<string>(), j.value("requestId", ""), j.value("customerId", ""),
              j.value("hallId", ""), dateFromJson(j.at("start")), dateFromJson(j.at("end")),
              j.value("guestCount", 0), j.value("estimatedTotal", int64_t(0)),
              bookingStatusFromString(j.value("status", "Đang chờ đặt cọc")));
    b.setDiscount(j.value("discountPercent", 0), j.value("discountAmount", int64_t(0)),
                  j.value("discountNeedsApproval", false));
    b.setDiscountApproved(j.value("discountApproved", true));
    if (j.contains("createdAt")) b.setCreatedAt(dateFromJson(j.at("createdAt")));
    if (j.contains("holdUntil")) b.setHoldUntil(dateFromJson(j.at("holdUntil")));
    b.setSalesUsername(j.value("salesUsername", ""));
    b.setCancelReason(j.value("cancelReason", ""));
    return b;
}

}  // namespace tdp
