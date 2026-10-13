#include "models/EventRequest.hpp"

using namespace std;

namespace tdp {

EventRequest::EventRequest(string id, string customerId, EventType type, string title, DateTime start,
                           DateTime end, int guestCount, string specialRequests, RequestStatus status)
    : id_(move(id)),
      customerId_(move(customerId)),
      title_(move(title)),
      specialRequests_(move(specialRequests)),
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
    return EventRequest(j.at("id").get<string>(), j.at("customerId").get<string>(),
                        eventTypeFromString(j.value("type", "TiecCuoi")), j.value("title", ""),
                        dateFromJson(j.at("start")), dateFromJson(j.at("end")), j.value("guestCount", 0),
                        j.value("specialRequests", ""), requestStatusFromString(j.value("status", "TiepNhan")));
}

}  // namespace tdp
