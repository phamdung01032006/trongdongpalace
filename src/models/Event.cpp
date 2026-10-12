#include "models/Event.hpp"

namespace tdp {

Event::Event(std::string id, std::string bookingId, std::string contractId, std::string customerId, std::string hallId,
             DateTime start, DateTime end, EventStatus status, std::string specialNotes)
    : id_(std::move(id)),
      bookingId_(std::move(bookingId)),
      contractId_(std::move(contractId)),
      customerId_(std::move(customerId)),
      hallId_(std::move(hallId)),
      specialNotes_(std::move(specialNotes)),
      start_(start),
      end_(end),
      status_(status) {}

nlohmann::json Event::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"contractId", contractId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"status", toString(status_)},
            {"specialNotes", specialNotes_}};
}

Event Event::fromJson(const nlohmann::json& j) {
    return Event(j.at("id").get<std::string>(), j.at("bookingId").get<std::string>(), j.value("contractId", ""),
                 j.at("customerId").get<std::string>(), j.at("hallId").get<std::string>(), dateFromJson(j.at("start")),
                 dateFromJson(j.at("end")), eventStatusFromString(j.value("status", "ChuanBi")),
                 j.value("specialNotes", ""));
}

}  // namespace tdp
