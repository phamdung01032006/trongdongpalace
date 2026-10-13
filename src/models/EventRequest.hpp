#pragma once

#include "models/Enums.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using namespace std;

namespace tdp {

// Dong dich vu khach chon trong yeu cau (dac ta A.2): luu don gia tai thoi
// diem chon + so luong + thanh tien (dac ta 2.9).
class RequestServiceLine {
public:
    RequestServiceLine() = default;
    RequestServiceLine(string serviceId, string nameSnapshot, string unitSnapshot, int quantity,
                       int64_t unitPrice);
    const string& serviceId() const { return serviceId_; }
    const string& nameSnapshot() const { return nameSnapshot_; }
    const string& unitSnapshot() const { return unitSnapshot_; }
    int quantity() const { return quantity_; }
    int64_t unitPrice() const { return unitPrice_; }
    int64_t amount() const { return static_cast<int64_t>(quantity_) * unitPrice_; }
    void setQuantity(int v) { quantity_ = v; }
    nlohmann::json toJson() const;
    static RequestServiceLine fromJson(const nlohmann::json& j);

private:
    string serviceId_, nameSnapshot_, unitSnapshot_;
    int quantity_ = 1;
    int64_t unitPrice_ = 0;
};

class EventRequest {
public:
    EventRequest() = default;
    EventRequest(string id, string customerId, EventType type, string title, string customerName,
                 string customerPhone, string hallId, DateTime start, DateTime end, int guestCount,
                 string notes, RequestStatus status);

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
    void setTitle(string v) { title_ = move(v); }
    void setCustomerName(string v) { customerName_ = move(v); }
    void setCustomerPhone(string v) { customerPhone_ = move(v); }
    void setHallId(string v) { hallId_ = move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setNotes(string v) { notes_ = move(v); }
    void setStatus(RequestStatus v) { status_ = v; }
    void setCreatedAt(DateTime v) { createdAt_ = v; }
    DateTime createdAt() const { return createdAt_; }

    int64_t servicesTotal() const;

    nlohmann::json toJson() const;
    static EventRequest fromJson(const nlohmann::json& j);

private:
    string id_, customerId_, title_, customerName_, customerPhone_, hallId_, notes_;
    vector<RequestServiceLine> lines_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_, end_, createdAt_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::Cho;
};

}  // namespace tdp
