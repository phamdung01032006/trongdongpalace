#pragma once

// Su kien duoc tu dong sinh ra khi booking duoc thu tien coc.
// Trang thai (Sap dien ra / Dang dien ra / Da ket thuc) duoc tinh lai
// moi khi nguoi dung xem danh sach, dua vao dong ho he thong.

#include "DateTime.h"
#include "Enums.h"

#include <string>

using namespace std;

class Event {
public:
    Event(string id = "", string bookingId = "", string customerId = "", string hallId = "",
          DateTime start = DateTime{}, DateTime end = DateTime{},
          EventStatus status = EventStatus::SapDienRa, string specialNotes = "");

    const string& get_id() const { return id_; }
    const string& get_bookingId() const { return bookingId_; }
    const string& get_customerId() const { return customerId_; }
    const string& get_hallId() const { return hallId_; }
    const DateTime& get_start() const { return start_; }
    const DateTime& get_end() const { return end_; }
    EventStatus get_status() const { return status_; }
    const string& get_specialNotes() const { return specialNotes_; }

    void set_status(EventStatus value) { status_ = value; }
    void set_specialNotes(const string& value) { specialNotes_ = value; }

    string toText() const;
    static Event fromText(const string& line);

private:
    string id_;
    string bookingId_;
    string customerId_;
    string hallId_;
    string specialNotes_;
    DateTime start_;
    DateTime end_;
    EventStatus status_ = EventStatus::SapDienRa;
};
