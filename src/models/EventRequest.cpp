#ifndef TDP_MODELS_EVENTREQUEST_CPP
#define TDP_MODELS_EVENTREQUEST_CPP

#include "models/Enums.cpp"

#include <cstdint>
#include "utils/text_fields.cpp"
#include <string>
#include <vector>

using namespace std;

// Dong dich vu khach chon trong yeu cau (dac ta A.2): luu don gia tai thoi
// diem chon + so luong + thanh tien (dac ta 2.9).
class RequestServiceLine {
public:
    RequestServiceLine() = default;

    RequestServiceLine(const string& serviceId, const string& nameSnapshot, const string& unitSnapshot,
                       int quantity, int64_t unitPrice)
        : serviceId_(serviceId),
          nameSnapshot_(nameSnapshot),
          unitSnapshot_(unitSnapshot),
          quantity_(quantity),
          unitPrice_(unitPrice) {}

    const string& serviceId() const { return serviceId_; }
    const string& nameSnapshot() const { return nameSnapshot_; }
    const string& unitSnapshot() const { return unitSnapshot_; }
    int quantity() const { return quantity_; }
    int64_t unitPrice() const { return unitPrice_; }
    int64_t amount() const { return static_cast<int64_t>(quantity_) * unitPrice_; }
    void setQuantity(int v) { quantity_ = v; }
    vector<string> toFields() const;
    static RequestServiceLine fromFields(const vector<string>& f);

private:
    string serviceId_, nameSnapshot_, unitSnapshot_;
    int quantity_ = 1;
    int64_t unitPrice_ = 0;
};

class EventRequest {
public:
    EventRequest() = default;

    EventRequest(const string& id, const string& customerId, EventType type, const string& title,
                 const string& customerName, const string& customerPhone, const string& hallId,
                 DateTime start, DateTime end, int guestCount, const string& notes,
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
          guestCount_(guestCount),
          status_(status) {}

    const string& id() const { return id_; }
    const string& customerId() const { return customerId_; }
    EventType type() const { return type_; }
    const string& title() const { return title_; }
    const string& customerName() const { return customerName_; }
    const string& customerPhone() const { return customerPhone_; }
    // Sanh mong muon; rong = "Chua xac dinh - Can tu van" (Form 1).
    const string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    const string& notes() const { return notes_; }
    RequestStatus status() const { return status_; }
    const vector<RequestServiceLine>& lines() const { return lines_; }
    vector<RequestServiceLine>& lines() { return lines_; }

    void setType(EventType v) { type_ = v; }
    void setTitle(const string& value) { title_ = value; }
    void setCustomerName(const string& value) { customerName_ = value; }
    void setCustomerPhone(const string& value) { customerPhone_ = value; }
    void setHallId(const string& value) { hallId_ = value; }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setNotes(const string& value) { notes_ = value; }
    void setStatus(RequestStatus v) { status_ = v; }
    void setCreatedAt(DateTime v) { createdAt_ = v; }
    DateTime createdAt() const { return createdAt_; }

    int64_t servicesTotal() const {
        int64_t sum = 0;
        for (const auto& l : lines_) sum += l.amount();
        return sum;
    }

    vector<string> toFields() const;
    static EventRequest fromFields(const vector<string>& f);

private:
    string id_, customerId_, title_, customerName_, customerPhone_, hallId_, notes_;
    vector<RequestServiceLine> lines_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_, end_, createdAt_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::Cho;
};

vector<string> RequestServiceLine::toFields() const {
    vector<string> f = {serviceId_, nameSnapshot_, unitSnapshot_, to_string(quantity_), to_string(unitPrice_)};
    return f;
}

RequestServiceLine RequestServiceLine::fromFields(const vector<string>& f) {
    requireFieldCount(f, 5);
    RequestServiceLine item;
    item.serviceId_ = f.at(0);
    item.nameSnapshot_ = f.at(1);
    item.unitSnapshot_ = f.at(2);
    item.quantity_ = textInt(f.at(3));
    item.unitPrice_ = textMoney(f.at(4));
    return item;
}

vector<string> EventRequest::toFields() const {
    vector<string> f = {id_, customerId_, toString(type_), title_, customerName_, customerPhone_, hallId_, start_.toString(), end_.toString(), to_string(guestCount_), notes_, createdAt_.toString(), toString(status_)};
    f.push_back(to_string(lines_.size()));
    for (const auto& line : lines_) {
        auto fields = line.toFields();
        f.insert(f.end(), fields.begin(), fields.end());
    }
    return f;
}

EventRequest EventRequest::fromFields(const vector<string>& f) {
    if (f.size() < 14) throw ValidationException("Yeu cau thieu cot");
    int count = textInt(f.at(13));
    if (count < 0 || static_cast<size_t>(count) != (f.size() - 14) / 5 || (f.size() - 14) % 5 != 0)
        throw ValidationException("So dong dich vu khong hop le");
    EventRequest item;
    item.id_ = f.at(0);
    item.customerId_ = f.at(1);
    item.type_ = eventTypeFromString(f.at(2));
    if (toString(item.type_) != f.at(2)) throw ValidationException("Trang thai/loai khong hop le");
    item.title_ = f.at(3);
    item.customerName_ = f.at(4);
    item.customerPhone_ = f.at(5);
    item.hallId_ = f.at(6);
    item.start_ = textDate(f.at(7));
    item.end_ = textDate(f.at(8));
    item.guestCount_ = textInt(f.at(9));
    item.notes_ = f.at(10);
    item.createdAt_ = textDate(f.at(11));
    item.status_ = requestStatusFromString(f.at(12));
    if (toString(item.status_) != f.at(12)) throw ValidationException("Trang thai/loai khong hop le");
    for (size_t i = 14; i < f.size(); i += 5)
        item.lines_.push_back(RequestServiceLine::fromFields(vector<string>(f.begin() + i, f.begin() + i + 5)));
    return item;
}

#endif
