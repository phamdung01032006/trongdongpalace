#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class Booking {
public:
    Booking() = default;
    Booking(string id, string quotationId, string requestId, string customerId,
            string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status);

    const string& id() const { return id_; }
    const string& quotationId() const { return quotationId_; }
    const string& requestId() const { return requestId_; }
    const string& customerId() const { return customerId_; }
    const string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    BookingStatus status() const { return status_; }
    const string& cancelReason() const { return cancelReason_; }

    void setHallId(string v) { hallId_ = move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setStatus(BookingStatus v) { status_ = v; }
    void setCancelReason(string v) { cancelReason_ = move(v); }

    bool occupiesHall() const { return status_ != BookingStatus::DaHuy; }

    nlohmann::json toJson() const;
    static Booking fromJson(const nlohmann::json& j);

private:
    string id_, quotationId_, requestId_, customerId_, hallId_, cancelReason_;
    DateTime start_, end_;
    int guestCount_ = 0;
    BookingStatus status_ = BookingStatus::Nhat;
};

}  // namespace tdp
