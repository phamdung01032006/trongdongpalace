#include "Event.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Event::Event(string id, string bookingId, string customerId, string hallId,
             DateTime start, DateTime end, EventStatus status, string specialNotes)
    : id_(id),
      bookingId_(bookingId),
      customerId_(customerId),
      hallId_(hallId),
      specialNotes_(specialNotes),
      start_(start),
      end_(end),
      status_(status) {}

string Event::toText() const {
    // id | bookingId | customerId | hallId | bat dau | ket thuc | trang thai | ghi chu
    vector<string> fields = {
        cleanField(id_),
        cleanField(bookingId_),
        cleanField(customerId_),
        cleanField(hallId_),
        start_.toString(),
        end_.toString(),
        toString(status_),
        cleanField(specialNotes_)
    };
    return joinText(fields, '|');
}

Event Event::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 8) {
        throw FileException("Dong events.txt khong du truong: " + line);
    }

    return Event(fieldAt(fields, 0), fieldAt(fields, 1), fieldAt(fields, 2),
                 fieldAt(fields, 3), parseDateOr(fieldAt(fields, 4)),
                 parseDateOr(fieldAt(fields, 5)), eventStatusFromString(fieldAt(fields, 6)),
                 fieldAt(fields, 7));
}
