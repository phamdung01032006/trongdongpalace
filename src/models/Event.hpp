#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Event {
public:
    Event() = default;
    Event(string id, string bookingId, string customerId, string hallId,
          DateTime start, DateTime end, EventStatus status, string specialNotes);

    const string& id() const { return id_; }
    const string& bookingId() const { return bookingId_; }
    const string& customerId() const { return customerId_; }
    const string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    EventStatus status() const { return status_; }
    const string& specialNotes() const { return specialNotes_; }
    void setStatus(EventStatus v) { status_ = v; }
    void setSpecialNotes(string v) { specialNotes_ = move(v); }

    nlohmann::json toJson() const;
    static Event fromJson(const nlohmann::json& j);

private:
    string id_, bookingId_, customerId_, hallId_, specialNotes_;
    DateTime start_, end_;
    EventStatus status_ = EventStatus::ChuanBi;
};

}  // namespace tdp
