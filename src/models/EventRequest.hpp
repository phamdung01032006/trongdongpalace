#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class EventRequest {
public:
    EventRequest() = default;
    EventRequest(std::string id, std::string customerId, EventType type, std::string title, DateTime start,
                 DateTime end, int guestCount, std::string specialRequests, RequestStatus status);

    const std::string& id() const { return id_; }
    const std::string& customerId() const { return customerId_; }
    EventType type() const { return type_; }
    const std::string& title() const { return title_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    const std::string& specialRequests() const { return specialRequests_; }
    RequestStatus status() const { return status_; }

    void setType(EventType v) { type_ = v; }
    void setTitle(std::string v) { title_ = std::move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setSpecialRequests(std::string v) { specialRequests_ = std::move(v); }
    void setStatus(RequestStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static EventRequest fromJson(const nlohmann::json& j);

private:
    std::string id_, customerId_, title_, specialRequests_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_, end_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::TiepNhan;
};

}  // namespace tdp
