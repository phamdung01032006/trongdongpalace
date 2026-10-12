#include "models/Booking.hpp"

namespace tdp {

Booking::Booking(std::string id, std::string quotationId, std::string requestId, std::string customerId,
                 std::string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status)
    : id_(std::move(id)),
      quotationId_(std::move(quotationId)),
      requestId_(std::move(requestId)),
      customerId_(std::move(customerId)),
      hallId_(std::move(hallId)),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      status_(status) {}

nlohmann::json Booking::toJson() const {
    return {{"id", id_},
            {"quotationId", quotationId_},
            {"requestId", requestId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"status", toString(status_)},
            {"cancelReason", cancelReason_}};
}

Booking Booking::fromJson(const nlohmann::json& j) {
    Booking b(j.at("id").get<std::string>(), j.at("quotationId").get<std::string>(), j.at("requestId").get<std::string>(),
              j.at("customerId").get<std::string>(), j.at("hallId").get<std::string>(), dateFromJson(j.at("start")),
              dateFromJson(j.at("end")), j.value("guestCount", 0), bookingStatusFromString(j.value("status", "Nhat")));
    b.setCancelReason(j.value("cancelReason", ""));
    return b;
}

}  // namespace tdp
