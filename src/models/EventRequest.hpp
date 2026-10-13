#pragma once

#include "models/Enums.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class EventRequest {
public:
    EventRequest() = default;
    EventRequest(string id, string customerId, EventType type, string title, DateTime start,
                 DateTime end, int guestCount, string specialRequests, RequestStatus status);

    const string& id() const { return id_; }
    const string& customerId() const { return customerId_; }
    EventType type() const { return type_; }
    const string& title() const { return title_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    const string& specialRequests() const { return specialRequests_; }
    RequestStatus status() const { return status_; }

    void setType(EventType v) { type_ = v; }
    void setTitle(string v) { title_ = move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setSpecialRequests(string v) { specialRequests_ = move(v); }
    void setStatus(RequestStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static EventRequest fromJson(const nlohmann::json& j);

private:
    string id_, customerId_, title_, specialRequests_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_, end_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::TiepNhan;
};

}  // namespace tdp
