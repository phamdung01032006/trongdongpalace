#pragma once

// Booking la bang chung giu sanh cho khach trong 72 gio.
// Trang thai:
//   ChoCoc -> (thu tien coc) -> DaCoc -> (tat toan) -> HoanTat
//   ChoCoc -> (qua 72h chua coc) -> DaHuy
//
// Chiet khau > 10% phai duoc Quan ly duyet.

#include "DateTime.h"
#include "Enums.h"

#include <cstdint>
#include <string>

using namespace std;

class Booking {
public:
    Booking(string id = "", string requestId = "", string customerId = "", string hallId = "",
            DateTime start = DateTime{}, DateTime end = DateTime{}, int guestCount = 0,
            int64_t estimatedTotal = 0, BookingStatus status = BookingStatus::ChoCoc);

    const string& get_id() const { return id_; }
    const string& get_requestId() const { return requestId_; }
    const string& get_customerId() const { return customerId_; }
    const string& get_hallId() const { return hallId_; }
    const DateTime& get_start() const { return start_; }
    const DateTime& get_end() const { return end_; }
    int get_guestCount() const { return guestCount_; }
    int64_t get_estimatedTotal() const { return estimatedTotal_; }
    int get_discountPercent() const { return discountPercent_; }
    int64_t get_discountAmount() const { return discountAmount_; }
    bool get_discountNeedsApproval() const { return discountNeedsApproval_; }
    bool get_discountApproved() const { return discountApproved_; }
    BookingStatus get_status() const { return status_; }
    const DateTime& get_createdAt() const { return createdAt_; }
    const DateTime& get_holdUntil() const { return holdUntil_; }
    const string& get_salesUsername() const { return salesUsername_; }
    const string& get_cancelReason() const { return cancelReason_; }

    int64_t get_finalTotal() const;

    void set_hallId(const string& value) { hallId_ = value; }
    void set_start(const DateTime& value) { start_ = value; }
    void set_end(const DateTime& value) { end_ = value; }
    void set_guestCount(int value) { guestCount_ = value; }
    void set_estimatedTotal(int64_t value) { estimatedTotal_ = value; }
    void set_discount(int percent, int64_t amount, bool needsApproval);
    void set_discountApproved(bool value) { discountApproved_ = value; }
    void set_status(BookingStatus value) { status_ = value; }
    void set_createdAt(const DateTime& value) { createdAt_ = value; }
    void set_holdUntil(const DateTime& value) { holdUntil_ = value; }
    void set_salesUsername(const string& value) { salesUsername_ = value; }
    void set_cancelReason(const string& value) { cancelReason_ = value; }

    // Booking dang ChoCoc, DaCoc hoac da thanh toan (HoanTat) thi van chiem sanh.
    // Chi khi DaHuy moi giai phong sanh.
    bool is_occupying_hall() const {
        return status_ == BookingStatus::ChoCoc || status_ == BookingStatus::DaCoc ||
               status_ == BookingStatus::HoanTat;
    }

    string toText() const;
    static Booking fromText(const string& line);

private:
    string id_;
    string requestId_;
    string customerId_;
    string hallId_;
    string salesUsername_;
    string cancelReason_;
    DateTime start_;
    DateTime end_;
    DateTime createdAt_;
    DateTime holdUntil_;
    int guestCount_ = 0;
    int64_t estimatedTotal_ = 0;
    int discountPercent_ = 0;
    int64_t discountAmount_ = 0;
    bool discountNeedsApproval_ = false;
    bool discountApproved_ = false;
    BookingStatus status_ = BookingStatus::ChoCoc;
};
