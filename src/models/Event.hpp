#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Event {
public:
    Event() = default;
    Event(std::string id, std::string bookingId, std::string contractId, std::string customerId, std::string hallId,
          DateTime start, DateTime end, EventStatus status, std::string specialNotes);

    const std::string& id() const { return id_; }
    const std::string& bookingId() const { return bookingId_; }
    const std::string& contractId() const { return contractId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    EventStatus status() const { return status_; }
    const std::string& specialNotes() const { return specialNotes_; }
    void setContractId(std::string v) { contractId_ = std::move(v); }
    void setStatus(EventStatus v) { status_ = v; }
    void setSpecialNotes(std::string v) { specialNotes_ = std::move(v); }

    nlohmann::json toJson() const;
    static Event fromJson(const nlohmann::json& j);

private:
    std::string id_, bookingId_, contractId_, customerId_, hallId_, specialNotes_;
    DateTime start_, end_;
    EventStatus status_ = EventStatus::ChuanBi;
};

}  // namespace tdp
