#ifndef TDP_MODELS_EVENT_CPP
#define TDP_MODELS_EVENT_CPP

#include "models/Enums.cpp"
#include "utils/helpers.cpp"

#include "utils/text_fields.cpp"
#include <string>
#include <memory>

using namespace std;

class Event {
public:
    Event() = default;

    Event(const string& id, const string& bookingId, const string& customerId, const string& hallId,
          DateTime start, DateTime end, EventStatus status, const string& specialNotes)
        : id_(id),
          bookingId_(bookingId),
          customerId_(customerId),
          hallId_(hallId),
          specialNotes_(specialNotes),
          start_(start),
          end_(end),
          status_(status) {}

    virtual ~Event() = default;
    virtual EventType getEventType() const = 0;
    virtual string preparationGuide() const = 0;

    const string& id() const { return id_; }
    const string& bookingId() const { return bookingId_; }
    const string& customerId() const { return customerId_; }
    const string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    EventStatus status() const { return status_; }
    const string& specialNotes() const { return specialNotes_; }
    void setStatus(EventStatus v) { status_ = v; }
    void setSpecialNotes(const string& value) { specialNotes_ = value; }

    vector<string> toFields() const;
    static shared_ptr<Event> fromFields(const vector<string>& f);

private:
    string id_, bookingId_, customerId_, hallId_, specialNotes_;
    DateTime start_, end_;
    EventStatus status_ = EventStatus::SapDienRa;
};

class WeddingEvent : public Event {
public:
    WeddingEvent() = default;
    WeddingEvent(const string& id, const string& bookingId, const string& customerId,
                 const string& hallId, DateTime start, DateTime end, EventStatus status,
                 const string& notes)
        : Event(id, bookingId, customerId, hallId, start, end, status, notes) {}

    EventType getEventType() const override { return EventType::TiecCuoi; }
    string preparationGuide() const override {
        return "Chuan bi le duong, ban gia tien va kich ban don co dau chu re.";
    }
};

class BirthdayEvent : public Event {
public:
    BirthdayEvent() = default;
    BirthdayEvent(const string& id, const string& bookingId, const string& customerId,
                  const string& hallId, DateTime start, DateTime end, EventStatus status,
                  const string& notes)
        : Event(id, bookingId, customerId, hallId, start, end, status, notes) {}

    EventType getEventType() const override { return EventType::SinhNhat; }
    string preparationGuide() const override {
        return "Chuan bi banh sinh nhat, nen va trang tri theo do tuoi.";
    }
};

class CorporateEvent : public Event {
public:
    CorporateEvent(EventType type, const string& id, const string& bookingId, const string& customerId,
                   const string& hallId, DateTime start, DateTime end, EventStatus status, const string& notes)
        : Event(id, bookingId, customerId, hallId, start, end, status, notes), type_(type) {}
    EventType getEventType() const override { return type_; }
    string preparationGuide() const override {
        return "Chuan bi may chieu, am thanh, bang ten va khu vuc check-in.";
    }
private:
    EventType type_;
};

shared_ptr<Event> makeEvent(EventType type, const string& id, const string& bookingId,
                           const string& customerId, const string& hallId, DateTime start,
                           DateTime end, EventStatus status, const string& notes) {
    if (type == EventType::TiecCuoi)
        return make_shared<WeddingEvent>(id, bookingId, customerId, hallId, start, end, status, notes);
    if (type == EventType::SinhNhat)
        return make_shared<BirthdayEvent>(id, bookingId, customerId, hallId, start, end, status, notes);
    return make_shared<CorporateEvent>(type, id, bookingId, customerId, hallId, start, end, status, notes);
}

vector<string> Event::toFields() const {
    return {id_, bookingId_, customerId_, hallId_, start_.toString(), end_.toString(),
            toString(status_), specialNotes_, toString(getEventType())};
}

shared_ptr<Event> Event::fromFields(const vector<string>& f) {
    requireFieldCount(f, 9);
    auto type = eventTypeFromString(f[8]);
    auto status = eventStatusFromString(f[6]);
    if (toString(type) != f[8] || toString(status) != f[6])
        throw ValidationException("Loai/trang thai su kien khong hop le");
    return makeEvent(type, f[0], f[1], f[2], f[3], textDate(f[4]), textDate(f[5]), status, f[7]);
}

#endif
