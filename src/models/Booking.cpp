#include "models/Booking.hpp"

using namespace std;

namespace tdp {

Booking::Booking(string id, string quotationId, string requestId, string customerId,
                 string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status)
    : id_(move(id)),
      quotationId_(move(quotationId)),
      requestId_(move(requestId)),
      customerId_(move(customerId)),
      hallId_(move(hallId)),
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
    Booking b(j.at("id").get<string>(), j.at("quotationId").get<string>(), j.at("requestId").get<string>(),
              j.at("customerId").get<string>(), j.at("hallId").get<string>(), dateFromJson(j.at("start")),
              dateFromJson(j.at("end")), j.value("guestCount", 0), bookingStatusFromString(j.value("status", "Nhat")));
    b.setCancelReason(j.value("cancelReason", ""));
    return b;
}

}  // namespace tdp
