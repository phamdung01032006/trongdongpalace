#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

// Dat lich (dac ta B.2 + 3.x + Form 2): giu cho 72h cho coc, chiet khau co
// nguong duyet Quan ly, gia Booking = gia yeu cau tuong ung.
class Booking {
public:
    Booking() = default;
    Booking(string id, string requestId, string customerId, string hallId, DateTime start,
            DateTime end, int guestCount, int64_t estimatedTotal, BookingStatus status);

    const string& id() const { return id_; }
    const string& requestId() const { return requestId_; }
    const string& customerId() const { return customerId_; }
    const string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    int64_t estimatedTotal() const { return estimatedTotal_; }
    int discountPercent() const { return discountPercent_; }
    int64_t discountAmount() const { return discountAmount_; }
    int64_t finalTotal() const { return estimatedTotal_ - discountAmount_; }
    bool discountNeedsApproval() const { return discountNeedsApproval_; }
    bool discountApproved() const { return discountApproved_; }
    BookingStatus status() const { return status_; }
    DateTime createdAt() const { return createdAt_; }
    DateTime holdUntil() const { return holdUntil_; }
    const string& salesUsername() const { return salesUsername_; }
    const string& cancelReason() const { return cancelReason_; }

    void setHallId(string v) { hallId_ = move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setEstimatedTotal(int64_t v) { estimatedTotal_ = v; }
    void setDiscount(int percent, int64_t amount, bool needsApproval);
    void setDiscountApproved(bool v) { discountApproved_ = v; }
    void setStatus(BookingStatus v) { status_ = v; }
    void setCreatedAt(DateTime v) { createdAt_ = v; }
    void setHoldUntil(DateTime v) { holdUntil_ = v; }
    void setSalesUsername(string v) { salesUsername_ = move(v); }
    void setCancelReason(string v) { cancelReason_ = move(v); }

    // Chiem sanh khi Dang giu cho hoac Da coc (dac ta 3.2).
    bool occupiesHall() const { return status_ == BookingStatus::ChoCoc || status_ == BookingStatus::DaCoc; }

    nlohmann::json toJson() const;
    static Booking fromJson(const nlohmann::json& j);

private:
    string id_, requestId_, customerId_, hallId_, salesUsername_, cancelReason_;
    DateTime start_, end_, createdAt_, holdUntil_;
    int guestCount_ = 0;
    int64_t estimatedTotal_ = 0;
    int discountPercent_ = 0;
    int64_t discountAmount_ = 0;
    bool discountNeedsApproval_ = false;
    bool discountApproved_ = false;
    BookingStatus status_ = BookingStatus::ChoCoc;
};

}  // namespace tdp
