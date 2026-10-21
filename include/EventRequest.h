#pragma once

// Mot yeu cau dat lich su kien (Form 1).
// Khach hang gui yeu cau -> Kinh doanh tu van -> Hoan thanh -> chuyen thanh Booking.

#include "DateTime.h"
#include "Enums.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

// Mot dong dich vu khach da chon.
// Ta luu lai TEN, DON VI va DON GIA tai thoi diem chon (snapshot).
// Nho vay, neu Quan ly sua bang gia sau nay thi yeu cau cu khong bi lech tien.
class RequestLine {
public:
    RequestLine(string serviceId = "", string nameSnapshot = "", string unitSnapshot = "",
                int quantity = 1, int64_t unitPrice = 0);

    const string& get_serviceId() const { return serviceId_; }
    const string& get_nameSnapshot() const { return nameSnapshot_; }
    const string& get_unitSnapshot() const { return unitSnapshot_; }
    int get_quantity() const { return quantity_; }
    int64_t get_unitPrice() const { return unitPrice_; }
    int64_t get_amount() const { return static_cast<int64_t>(quantity_) * unitPrice_; }

    void set_quantity(int value) { quantity_ = value; }

    string toText() const;
    static RequestLine fromText(const string& line);

private:
    string serviceId_;
    string nameSnapshot_;
    string unitSnapshot_;
    int quantity_ = 1;
    int64_t unitPrice_ = 0;
};

class EventRequest {
public:
    EventRequest(string id = "", string customerId = "", EventType type = EventType::TiecCuoi,
                 string title = "", string customerName = "", string customerPhone = "",
                 string hallId = "", DateTime start = DateTime{}, DateTime end = DateTime{},
                 int guestCount = 0, string notes = "",
                 RequestStatus status = RequestStatus::Cho);

    const string& get_id() const { return id_; }
    const string& get_customerId() const { return customerId_; }
    EventType get_type() const { return type_; }
    const string& get_title() const { return title_; }
    const string& get_customerName() const { return customerName_; }
    const string& get_customerPhone() const { return customerPhone_; }
    const string& get_hallId() const { return hallId_; }
    const DateTime& get_start() const { return start_; }
    const DateTime& get_end() const { return end_; }
    int get_guestCount() const { return guestCount_; }
    const string& get_notes() const { return notes_; }
    RequestStatus get_status() const { return status_; }
    const DateTime& get_createdAt() const { return createdAt_; }

    void set_type(EventType value) { type_ = value; }
    void set_title(const string& value) { title_ = value; }
    void set_customerName(const string& value) { customerName_ = value; }
    void set_customerPhone(const string& value) { customerPhone_ = value; }
    void set_hallId(const string& value) { hallId_ = value; }
    void set_start(const DateTime& value) { start_ = value; }
    void set_end(const DateTime& value) { end_ = value; }
    void set_guestCount(int value) { guestCount_ = value; }
    void set_notes(const string& value) { notes_ = value; }
    void set_status(RequestStatus value) { status_ = value; }
    void set_createdAt(const DateTime& value) { createdAt_ = value; }

    const vector<RequestLine>& get_lines() const { return lines_; }
    vector<RequestLine>& get_lines() { return lines_; }

    int64_t get_servicesTotal() const;

    string toText() const;
    static EventRequest fromText(const string& line);

private:
    string id_;
    string customerId_;
    string title_;
    string customerName_;
    string customerPhone_;
    string hallId_;  // Rong = "chua xac dinh, can tu van".
    string notes_;
    vector<RequestLine> lines_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_;
    DateTime end_;
    DateTime createdAt_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::Cho;
};
