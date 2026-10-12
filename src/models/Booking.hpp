#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class Booking {
public:
    Booking() = default;
    Booking(std::string id, std::string quotationId, std::string requestId, std::string customerId,
            std::string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status);

    const std::string& id() const { return id_; }
    const std::string& quotationId() const { return quotationId_; }
    const std::string& requestId() const { return requestId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    BookingStatus status() const { return status_; }
    const std::string& cancelReason() const { return cancelReason_; }

    void setHallId(std::string v) { hallId_ = std::move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setStatus(BookingStatus v) { status_ = v; }
    void setCancelReason(std::string v) { cancelReason_ = std::move(v); }

    bool occupiesHall() const { return status_ != BookingStatus::DaHuy; }

    nlohmann::json toJson() const;
    static Booking fromJson(const nlohmann::json& j);

private:
    std::string id_, quotationId_, requestId_, customerId_, hallId_, cancelReason_;
    DateTime start_, end_;
    int guestCount_ = 0;
    BookingStatus status_ = BookingStatus::Nhat;
};

}  // namespace tdp
