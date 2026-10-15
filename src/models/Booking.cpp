#ifndef TDP_MODELS_BOOKING_CPP
#define TDP_MODELS_BOOKING_CPP

#include "models/Enums.cpp"
#include "utils/helpers.cpp"

#include <cstdint>
#include "utils/text_fields.cpp"
#include <string>

using namespace std;

// Dat lich (dac ta B.2 + 3.x + Form 2): giu cho 72h cho coc, chiet khau co
// nguong duyet Quan ly, gia Booking = gia yeu cau tuong ung.
class Booking {
public:
    Booking() = default;

    Booking(const string& id, const string& requestId, const string& customerId, const string& hallId, DateTime start,
             DateTime end, int guestCount, int64_t estimatedTotal, BookingStatus status)
        : id_(id),
          requestId_(requestId),
          customerId_(customerId),
          hallId_(hallId),
          start_(start),
          end_(end),
          guestCount_(guestCount),
          estimatedTotal_(estimatedTotal),
          status_(status) {}

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

    void setHallId(const string& value) { hallId_ = value; }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setEstimatedTotal(int64_t v) { estimatedTotal_ = v; }
    void setDiscount(int percent, int64_t amount, bool needsApproval) {
        discountPercent_ = percent;
        discountAmount_ = amount;
        discountNeedsApproval_ = needsApproval;
        if (!needsApproval) discountApproved_ = true;
    }
    void setDiscountApproved(bool v) { discountApproved_ = v; }
    void setStatus(BookingStatus v) { status_ = v; }
    void setCreatedAt(DateTime v) { createdAt_ = v; }
    void setHoldUntil(DateTime v) { holdUntil_ = v; }
    void setSalesUsername(const string& value) { salesUsername_ = value; }
    void setCancelReason(const string& value) { cancelReason_ = value; }

    // Chiem sanh khi Dang giu cho hoac Da coc (dac ta 3.2).
    bool occupiesHall() const { return status_ == BookingStatus::ChoCoc || status_ == BookingStatus::DaCoc; }

    vector<string> toFields() const;
    static Booking fromFields(const vector<string>& f);

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

vector<string> Booking::toFields() const {
    vector<string> f = {id_, requestId_, customerId_, hallId_, start_.toString(), end_.toString(), to_string(guestCount_), to_string(estimatedTotal_), to_string(discountPercent_), to_string(discountAmount_), (discountNeedsApproval_ ? "1" : "0"), (discountApproved_ ? "1" : "0"), toString(status_), createdAt_.toString(), holdUntil_.toString(), salesUsername_, cancelReason_};
    return f;
}

Booking Booking::fromFields(const vector<string>& f) {
    requireFieldCount(f, 17);
    Booking item;
    item.id_ = f.at(0);
    item.requestId_ = f.at(1);
    item.customerId_ = f.at(2);
    item.hallId_ = f.at(3);
    item.start_ = textDate(f.at(4));
    item.end_ = textDate(f.at(5));
    item.guestCount_ = textInt(f.at(6));
    item.estimatedTotal_ = textMoney(f.at(7));
    item.discountPercent_ = textInt(f.at(8));
    item.discountAmount_ = textMoney(f.at(9));
    item.discountNeedsApproval_ = textBool(f.at(10));
    item.discountApproved_ = textBool(f.at(11));
    item.status_ = bookingStatusFromString(f.at(12));
    if (toString(item.status_) != f.at(12)) throw ValidationException("Trang thai/loai khong hop le");
    item.createdAt_ = textDate(f.at(13));
    item.holdUntil_ = textDate(f.at(14));
    item.salesUsername_ = f.at(15);
    item.cancelReason_ = f.at(16);
    return item;
}

#endif
