#include "EventRequest.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

RequestLine::RequestLine(string serviceId, string nameSnapshot, string unitSnapshot,
                         int quantity, int64_t unitPrice)
    : serviceId_(serviceId),
      nameSnapshot_(nameSnapshot),
      unitSnapshot_(unitSnapshot),
      quantity_(quantity),
      unitPrice_(unitPrice) {}

string RequestLine::toText() const {
    // serviceId | ten luc chon | don vi | so luong | don gia
    vector<string> fields = {
        cleanField(serviceId_),
        cleanField(nameSnapshot_),
        cleanField(unitSnapshot_),
        to_string(quantity_),
        to_string(unitPrice_)
    };
    return joinText(fields, '|');
}

RequestLine RequestLine::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 5) {
        throw FileException("Dong request_lines.txt khong du truong: " + line);
    }

    return RequestLine(fieldAt(fields, 0), fieldAt(fields, 1), fieldAt(fields, 2),
                       toInt(fieldAt(fields, 3), 1), toInt64(fieldAt(fields, 4)));
}

EventRequest::EventRequest(string id, string customerId, EventType type, string title,
                           string customerName, string customerPhone, string hallId,
                           DateTime start, DateTime end, int guestCount, string notes,
                           RequestStatus status)
    : id_(id),
      customerId_(customerId),
      title_(title),
      customerName_(customerName),
      customerPhone_(customerPhone),
      hallId_(hallId),
      notes_(notes),
      type_(type),
      start_(start),
      end_(end),
      createdAt_(DateTime::now()),
      guestCount_(guestCount),
      status_(status) {}

int64_t EventRequest::get_servicesTotal() const {
    int64_t total = 0;
    for (const RequestLine& line : lines_) total += line.get_amount();
    return total;
}

string EventRequest::toText() const {
    // id | customerId | loai | ten | ten khach | sdt | hallId | bat dau | ket thuc
    //    | so khach | ghi chu | trang thai | ngay tao
    vector<string> fields = {
        cleanField(id_),
        cleanField(customerId_),
        toString(type_),
        cleanField(title_),
        cleanField(customerName_),
        cleanField(customerPhone_),
        cleanField(hallId_),
        start_.toString(),
        end_.toString(),
        to_string(guestCount_),
        cleanField(notes_),
        toString(status_),
        createdAt_.toString()
    };
    return joinText(fields, '|');
}

EventRequest EventRequest::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 13) {
        throw FileException("Dong requests.txt khong du truong: " + line);
    }

    EventRequest request(fieldAt(fields, 0), fieldAt(fields, 1),
                         eventTypeFromString(fieldAt(fields, 2)), fieldAt(fields, 3),
                         fieldAt(fields, 4), fieldAt(fields, 5), fieldAt(fields, 6),
                         parseDateOr(fieldAt(fields, 7)), parseDateOr(fieldAt(fields, 8)),
                         toInt(fieldAt(fields, 9)), fieldAt(fields, 10),
                         requestStatusFromString(fieldAt(fields, 11)));
    request.set_createdAt(parseDateOr(fieldAt(fields, 12), DateTime::now()));
    return request;
}
