#include "models/Event.hpp"

using namespace std;

namespace tdp {

Event::Event(string id, string bookingId, string customerId, string hallId,
             DateTime start, DateTime end, EventStatus status, string specialNotes)
    : id_(move(id)),
      bookingId_(move(bookingId)),
      customerId_(move(customerId)),
      hallId_(move(hallId)),
      specialNotes_(move(specialNotes)),
      start_(start),
      end_(end),
      status_(status) {}

nlohmann::json Event::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"status", toString(status_)},
            {"specialNotes", specialNotes_}};
}

Event Event::fromJson(const nlohmann::json& j) {
    return Event(j.at("id").get<string>(), j.at("bookingId").get<string>(),
                 j.at("customerId").get<string>(), j.at("hallId").get<string>(), dateFromJson(j.at("start")),
                 dateFromJson(j.at("end")), eventStatusFromString(j.value("status", "ChuanBi")),
                 j.value("specialNotes", ""));
}

}  // namespace tdp
