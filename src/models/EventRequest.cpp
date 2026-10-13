#include "models/EventRequest.hpp"

using namespace std;

namespace tdp {

RequestServiceLine::RequestServiceLine(string serviceId, string nameSnapshot, string unitSnapshot,
                                       int quantity, int64_t unitPrice)
    : serviceId_(move(serviceId)),
      nameSnapshot_(move(nameSnapshot)),
      unitSnapshot_(move(unitSnapshot)),
      quantity_(quantity),
      unitPrice_(unitPrice) {}

nlohmann::json RequestServiceLine::toJson() const {
    return {{"serviceId", serviceId_},
            {"nameSnapshot", nameSnapshot_},
            {"unitSnapshot", unitSnapshot_},
            {"quantity", quantity_},
            {"unitPrice", unitPrice_}};
}

RequestServiceLine RequestServiceLine::fromJson(const nlohmann::json& j) {
    return RequestServiceLine(j.value("serviceId", ""), j.value("nameSnapshot", ""),
                              j.value("unitSnapshot", ""), j.value("quantity", 1),
                              j.value("unitPrice", int64_t(0)));
}

EventRequest::EventRequest(string id, string customerId, EventType type, string title,
                           string customerName, string customerPhone, string hallId,
                           DateTime start, DateTime end, int guestCount, string notes,
                           RequestStatus status)
    : id_(move(id)),
      customerId_(move(customerId)),
      title_(move(title)),
      customerName_(move(customerName)),
      customerPhone_(move(customerPhone)),
      hallId_(move(hallId)),
      notes_(move(notes)),
      type_(type),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      status_(status) {}

int64_t EventRequest::servicesTotal() const {
    int64_t sum = 0;
    for (const auto& l : lines_) sum += l.amount();
    return sum;
}

nlohmann::json EventRequest::toJson() const {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& l : lines_) arr.push_back(l.toJson());
    return {{"id", id_},
            {"customerId", customerId_},
            {"type", toString(type_)},
            {"title", title_},
            {"customerName", customerName_},
            {"customerPhone", customerPhone_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"notes", notes_},
            {"specialRequests", notes_},
            {"createdAt", dateToJson(createdAt_)},
            {"lines", arr},
            {"status", toString(status_)}};
}

EventRequest EventRequest::fromJson(const nlohmann::json& j) {
    EventRequest r(j.at("id").get<string>(), j.at("customerId").get<string>(),
                   eventTypeFromString(j.value("type", "Tiệc cưới")), j.value("title", ""),
                   j.value("customerName", ""), j.value("customerPhone", ""), j.value("hallId", ""),
                   dateFromJson(j.at("start")), dateFromJson(j.at("end")), j.value("guestCount", 0),
                   j.value("notes", j.value("specialRequests", "")),
                   requestStatusFromString(j.value("status", "Chờ")));
    if (j.contains("lines") && j.at("lines").is_array()) {
        for (const auto& item : j.at("lines")) r.lines().push_back(RequestServiceLine::fromJson(item));
    }
    if (j.contains("createdAt")) r.setCreatedAt(dateFromJson(j.at("createdAt")));
    return r;
}

}  // namespace tdp
