#include "Booking.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Booking::Booking(string id, string requestId, string customerId, string hallId,
                 DateTime start, DateTime end, int guestCount, int64_t estimatedTotal,
                 BookingStatus status)
    : id_(id),
      requestId_(requestId),
      customerId_(customerId),
      hallId_(hallId),
      start_(start),
      end_(end),
      createdAt_(DateTime::now()),
      guestCount_(guestCount),
      estimatedTotal_(estimatedTotal),
      status_(status) {}

int64_t Booking::get_finalTotal() const {
    return estimatedTotal_;
}

string Booking::toText() const {
    // 17 truong, xem thu tu ben duoi.
    vector<string> fields = {
        cleanField(id_),
        cleanField(requestId_),
        cleanField(customerId_),
        cleanField(hallId_),
        start_.toString(),
        end_.toString(),
        to_string(guestCount_),
        to_string(estimatedTotal_),
        toString(status_),
        createdAt_.toString(),
        holdUntil_.toString(),
        cleanField(salesUsername_),
        cleanField(cancelReason_)
    };
    return joinText(fields, '|');
}

Booking Booking::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 17) {
        throw FileException("Dong bookings.txt khong du truong: " + line);
    }

    Booking booking(fieldAt(fields, 0), fieldAt(fields, 1), fieldAt(fields, 2),
                    fieldAt(fields, 3), parseDateOr(fieldAt(fields, 4)),
                    parseDateOr(fieldAt(fields, 5)), toInt(fieldAt(fields, 6)),
                    toInt64(fieldAt(fields, 7)),
                    bookingStatusFromString(fieldAt(fields, 12)));

    booking.set_createdAt(parseDateOr(fieldAt(fields, 9), DateTime::now()));
    booking.set_holdUntil(parseDateOr(fieldAt(fields, 10)));
    booking.set_salesUsername(fieldAt(fields, 11));
    booking.set_cancelReason(fieldAt(fields, 12));
    return booking;
    return booking;
}
