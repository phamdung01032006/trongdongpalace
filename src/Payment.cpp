#include "Payment.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Payment::Payment(string id, string bookingId, PaymentType type, int64_t amount,
                 DateTime paidAt, PaymentMethod method, PaymentStatus status,
                 string note, string recordedBy)
    : id_(id),
      bookingId_(bookingId),
      note_(note),
      recordedBy_(recordedBy),
      amount_(amount),
      paidAt_(paidAt),
      type_(type),
      method_(method),
      status_(status) {}

string Payment::toText() const {
    // id | bookingId | loai | so tien | ngay thu | hinh thuc | trang thai | ghi chu | nguoi thu
    vector<string> fields = {
        cleanField(id_),
        cleanField(bookingId_),
        toString(type_),
        to_string(amount_),
        paidAt_.toString(),
        toString(method_),
        toString(status_),
        cleanField(note_),
        cleanField(recordedBy_)
    };
    return joinText(fields, '|');
}

Payment Payment::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 9) {
        throw FileException("Dong payments.txt khong du truong: " + line);
    }

    return Payment(fieldAt(fields, 0), fieldAt(fields, 1),
                   paymentTypeFromString(fieldAt(fields, 2)), toInt64(fieldAt(fields, 3)),
                   parseDateOr(fieldAt(fields, 4)), paymentMethodFromString(fieldAt(fields, 5)),
                   paymentStatusFromString(fieldAt(fields, 6)), fieldAt(fields, 7),
                   fieldAt(fields, 8));
}
