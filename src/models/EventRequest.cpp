#include "models/EventRequest.hpp"

namespace tdp {

EventRequest::EventRequest(std::string id, std::string customerId, EventType type, std::string title, DateTime start,
                           DateTime end, int guestCount, std::string specialRequests, RequestStatus status)
    : id_(std::move(id)),
      customerId_(std::move(customerId)),
      title_(std::move(title)),
      specialRequests_(std::move(specialRequests)),
      type_(type),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      status_(status) {}

nlohmann::json EventRequest::toJson() const {
    return {{"id", id_},
            {"customerId", customerId_},
            {"type", toString(type_)},
            {"title", title_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"specialRequests", specialRequests_},
            {"status", toString(status_)}};
}

EventRequest EventRequest::fromJson(const nlohmann::json& j) {
    return EventRequest(j.at("id").get<std::string>(), j.at("customerId").get<std::string>(),
                        eventTypeFromString(j.value("type", "TiecCuoi")), j.value("title", ""),
                        dateFromJson(j.at("start")), dateFromJson(j.at("end")), j.value("guestCount", 0),
                        j.value("specialRequests", ""), requestStatusFromString(j.value("status", "TiepNhan")));
}

}  // namespace tdp
