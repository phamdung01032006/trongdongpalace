#pragma once

#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tdp {

enum class Role { Admin, Sales, Accountant, Coordinator };
enum class EventType { TiecCuoi, HoiNghi, SinhNhat, SuKienDoanhNghiep, Khac };
enum class RequestStatus { TiepNhan, DaBaoGia, DaChuyenBooking, Huy, TuChoi };
enum class QuotationStatus { Nhat, DaGui, DaChapNhan, TuChoi };
enum class BookingStatus { Nhat, DaXacNhan, DaHuy };
enum class ContractStatus { HieuLuc, HoanTat, Huy };
enum class EventStatus { ChuanBi, DangDienRa, HoanTat, Huy };
enum class TaskStatus { CanLam, DangLam, HoanThanh };
enum class IncidentStatus { Mo, DangXuLy, DaXuLy };
enum class PaymentMethod { TienMat, ChuyenKhoan, The };

std::string toString(Role v);
std::string toString(EventType v);
std::string toString(RequestStatus v);
std::string toString(QuotationStatus v);
std::string toString(BookingStatus v);
std::string toString(ContractStatus v);
std::string toString(EventStatus v);
std::string toString(TaskStatus v);
std::string toString(IncidentStatus v);
std::string toString(PaymentMethod v);

Role roleFromString(const std::string& s);
EventType eventTypeFromString(const std::string& s);
RequestStatus requestStatusFromString(const std::string& s);
QuotationStatus quotationStatusFromString(const std::string& s);
BookingStatus bookingStatusFromString(const std::string& s);
ContractStatus contractStatusFromString(const std::string& s);
EventStatus eventStatusFromString(const std::string& s);
TaskStatus taskStatusFromString(const std::string& s);
IncidentStatus incidentStatusFromString(const std::string& s);
PaymentMethod paymentMethodFromString(const std::string& s);

nlohmann::json dateToJson(const DateTime& d);
DateTime dateFromJson(const nlohmann::json& j);

class User {
public:
    User() = default;
    User(std::string id, std::string username, std::string salt, std::string passwordHash,
         std::string fullName, Role role, bool active);

    const std::string& id() const { return id_; }
    const std::string& username() const { return username_; }
    const std::string& salt() const { return salt_; }
    const std::string& passwordHash() const { return passwordHash_; }
    const std::string& fullName() const { return fullName_; }
    Role role() const { return role_; }
    bool active() const { return active_; }
    // Mật khẩu dạng rõ chỉ phục vụ demo; đăng nhập vẫn kiểm tra bằng salt + hash.
    const std::string& demoPassword() const { return demoPassword_; }

    void setFullName(const std::string& v) { fullName_ = v; }
    void setRole(Role v) { role_ = v; }
    void setActive(bool v) { active_ = v; }
    void setDemoPassword(const std::string& v) { demoPassword_ = v; }
    void setPassword(const std::string& salt, const std::string& hash) {
        salt_ = salt;
        passwordHash_ = hash;
    }

    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& j);

private:
    std::string id_;
    std::string username_;
    std::string salt_;
    std::string passwordHash_;
    std::string fullName_;
    std::string demoPassword_;  // ghi rõ trong JSON để thuận tiện demo
    Role role_ = Role::Sales;
    bool active_ = true;
};

class Customer {
public:
    Customer() = default;
    Customer(std::string id, std::string name, std::string phone, std::string email, std::string address,
             std::string notes, DateTime createdAt);

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& phone() const { return phone_; }
    const std::string& email() const { return email_; }
    const std::string& address() const { return address_; }
    const std::string& notes() const { return notes_; }
    DateTime createdAt() const { return createdAt_; }

    void setName(std::string v) { name_ = std::move(v); }
    void setPhone(std::string v) { phone_ = std::move(v); }
    void setEmail(std::string v) { email_ = std::move(v); }
    void setAddress(std::string v) { address_ = std::move(v); }
    void setNotes(std::string v) { notes_ = std::move(v); }

    nlohmann::json toJson() const;
    static Customer fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, phone_, email_, address_, notes_;
    DateTime createdAt_;
};

class EventRequest {
public:
    EventRequest() = default;
    EventRequest(std::string id, std::string customerId, EventType type, std::string title, DateTime start,
                 DateTime end, int guestCount, std::string specialRequests, RequestStatus status);

    const std::string& id() const { return id_; }
    const std::string& customerId() const { return customerId_; }
    EventType type() const { return type_; }
    const std::string& title() const { return title_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    const std::string& specialRequests() const { return specialRequests_; }
    RequestStatus status() const { return status_; }

    void setType(EventType v) { type_ = v; }
    void setTitle(std::string v) { title_ = std::move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setSpecialRequests(std::string v) { specialRequests_ = std::move(v); }
    void setStatus(RequestStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static EventRequest fromJson(const nlohmann::json& j);

private:
    std::string id_, customerId_, title_, specialRequests_;
    EventType type_ = EventType::TiecCuoi;
    DateTime start_, end_;
    int guestCount_ = 0;
    RequestStatus status_ = RequestStatus::TiepNhan;
};

class Center {
public:
    Center() = default;
    Center(std::string id, std::string name, std::string address, bool active);
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& address() const { return address_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setAddress(std::string v) { address_ = std::move(v); }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Center fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, address_;
    bool active_ = true;
};

class Hall {
public:
    Hall() = default;
    Hall(std::string id, std::string centerId, std::string name, int capacity, bool active);
    const std::string& id() const { return id_; }
    const std::string& centerId() const { return centerId_; }
    const std::string& name() const { return name_; }
    int capacity() const { return capacity_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setCapacity(int v) { capacity_ = v; }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static Hall fromJson(const nlohmann::json& j);

private:
    std::string id_, centerId_, name_;
    int capacity_ = 0;
    bool active_ = true;
};

class ServiceItem {
public:
    ServiceItem() = default;
    ServiceItem(std::string id, std::string name, std::string unit, int64_t unitPrice, std::string category,
                bool active);
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& unit() const { return unit_; }
    int64_t unitPrice() const { return unitPrice_; }
    const std::string& category() const { return category_; }
    bool active() const { return active_; }
    void setName(std::string v) { name_ = std::move(v); }
    void setUnit(std::string v) { unit_ = std::move(v); }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    void setCategory(std::string v) { category_ = std::move(v); }
    void setActive(bool v) { active_ = v; }
    nlohmann::json toJson() const;
    static ServiceItem fromJson(const nlohmann::json& j);

private:
    std::string id_, name_, unit_, category_;
    int64_t unitPrice_ = 0;
    bool active_ = true;
};

class QuotationLine {
public:
    QuotationLine() = default;
    QuotationLine(std::string serviceId, std::string nameSnapshot, int quantity, int64_t unitPrice);
    const std::string& serviceId() const { return serviceId_; }
    const std::string& nameSnapshot() const { return nameSnapshot_; }
    int quantity() const { return quantity_; }
    int64_t unitPrice() const { return unitPrice_; }
    int64_t amount() const { return static_cast<int64_t>(quantity_) * unitPrice_; }
    void setQuantity(int v) { quantity_ = v; }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    nlohmann::json toJson() const;
    static QuotationLine fromJson(const nlohmann::json& j);

private:
    std::string serviceId_, nameSnapshot_;
    int quantity_ = 1;
    int64_t unitPrice_ = 0;
};

class Quotation {
public:
    Quotation() = default;
    Quotation(std::string id, std::string requestId, std::string customerId, std::string note);

    const std::string& id() const { return id_; }
    const std::string& requestId() const { return requestId_; }
    const std::string& customerId() const { return customerId_; }
    const std::vector<QuotationLine>& lines() const { return lines_; }
    std::vector<QuotationLine>& lines() { return lines_; }
    int discountPercent() const { return discountPercent_; }
    int64_t discountAmount() const { return discountAmount_; }
    int64_t subtotal() const { return subtotal_; }
    int64_t total() const { return total_; }
    QuotationStatus status() const { return status_; }
    const std::string& note() const { return note_; }

    void setStatus(QuotationStatus v) { status_ = v; }
    void setNote(std::string v) { note_ = std::move(v); }
    void recalc();

    nlohmann::json toJson() const;
    static Quotation fromJson(const nlohmann::json& j);

private:
    std::string id_, requestId_, customerId_, note_;
    std::vector<QuotationLine> lines_;
    int discountPercent_ = 0;
    int64_t discountAmount_ = 0;
    int64_t subtotal_ = 0;
    int64_t total_ = 0;
    QuotationStatus status_ = QuotationStatus::Nhat;
};

class Booking {
public:
    Booking() = default;
    Booking(std::string id, std::string quotationId, std::string requestId, std::string customerId,
            std::string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status);

    const std::string& id() const { return id_; }
    const std::string& quotationId() const { return quotationId_; }
    const std::string& requestId() const { return requestId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    int guestCount() const { return guestCount_; }
    BookingStatus status() const { return status_; }
    const std::string& cancelReason() const { return cancelReason_; }

    void setHallId(std::string v) { hallId_ = std::move(v); }
    void setStart(DateTime v) { start_ = v; }
    void setEnd(DateTime v) { end_ = v; }
    void setGuestCount(int v) { guestCount_ = v; }
    void setStatus(BookingStatus v) { status_ = v; }
    void setCancelReason(std::string v) { cancelReason_ = std::move(v); }

    bool occupiesHall() const { return status_ != BookingStatus::DaHuy; }

    nlohmann::json toJson() const;
    static Booking fromJson(const nlohmann::json& j);

private:
    std::string id_, quotationId_, requestId_, customerId_, hallId_, cancelReason_;
    DateTime start_, end_;
    int guestCount_ = 0;
    BookingStatus status_ = BookingStatus::Nhat;
};

class Contract {
public:
    Contract() = default;
    Contract(std::string id, std::string bookingId, std::string customerId, std::string contractNo,
             DateTime signedAt, int64_t totalAmount, ContractStatus status);

    const std::string& id() const { return id_; }
    const std::string& bookingId() const { return bookingId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& contractNo() const { return contractNo_; }
    DateTime signedAt() const { return signedAt_; }
    int64_t totalAmount() const { return totalAmount_; }
    ContractStatus status() const { return status_; }
    void setStatus(ContractStatus v) { status_ = v; }

    nlohmann::json toJson() const;
    static Contract fromJson(const nlohmann::json& j);

private:
    std::string id_, bookingId_, customerId_, contractNo_;
    DateTime signedAt_;
    int64_t totalAmount_ = 0;
    ContractStatus status_ = ContractStatus::HieuLuc;
};

class Payment {
public:
    Payment() = default;
    Payment(std::string id, std::string contractId, int64_t amount, DateTime paidAt, PaymentMethod method,
            std::string note, std::string recordedBy);

    const std::string& id() const { return id_; }
    const std::string& contractId() const { return contractId_; }
    int64_t amount() const { return amount_; }
    DateTime paidAt() const { return paidAt_; }
    PaymentMethod method() const { return method_; }
    const std::string& note() const { return note_; }
    const std::string& recordedBy() const { return recordedBy_; }

    nlohmann::json toJson() const;
    static Payment fromJson(const nlohmann::json& j);

private:
    std::string id_, contractId_, note_, recordedBy_;
    int64_t amount_ = 0;
    DateTime paidAt_;
    PaymentMethod method_ = PaymentMethod::ChuyenKhoan;
};

class Event {
public:
    Event() = default;
    Event(std::string id, std::string bookingId, std::string contractId, std::string customerId, std::string hallId,
          DateTime start, DateTime end, EventStatus status, std::string specialNotes);

    const std::string& id() const { return id_; }
    const std::string& bookingId() const { return bookingId_; }
    const std::string& contractId() const { return contractId_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& hallId() const { return hallId_; }
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
    EventStatus status() const { return status_; }
    const std::string& specialNotes() const { return specialNotes_; }
    void setContractId(std::string v) { contractId_ = std::move(v); }
    void setStatus(EventStatus v) { status_ = v; }
    void setSpecialNotes(std::string v) { specialNotes_ = std::move(v); }

    nlohmann::json toJson() const;
    static Event fromJson(const nlohmann::json& j);

private:
    std::string id_, bookingId_, contractId_, customerId_, hallId_, specialNotes_;
    DateTime start_, end_;
    EventStatus status_ = EventStatus::ChuanBi;
};

class OperationTask {
public:
    OperationTask() = default;
    OperationTask(std::string id, std::string eventId, std::string title, std::string assignee, DateTime due,
                  TaskStatus status, std::string note);
    const std::string& id() const { return id_; }
    const std::string& eventId() const { return eventId_; }
    const std::string& title() const { return title_; }
    const std::string& assignee() const { return assignee_; }
    DateTime due() const { return due_; }
    TaskStatus status() const { return status_; }
    const std::string& note() const { return note_; }
    void setTitle(std::string v) { title_ = std::move(v); }
    void setAssignee(std::string v) { assignee_ = std::move(v); }
    void setDue(DateTime v) { due_ = v; }
    void setStatus(TaskStatus v) { status_ = v; }
    void setNote(std::string v) { note_ = std::move(v); }
    nlohmann::json toJson() const;
    static OperationTask fromJson(const nlohmann::json& j);

private:
    std::string id_, eventId_, title_, assignee_, note_;
    DateTime due_;
    TaskStatus status_ = TaskStatus::CanLam;
};

class Incident {
public:
    Incident() = default;
    Incident(std::string id, std::string eventId, std::string description, IncidentStatus status,
             std::string resolution, DateTime createdAt);
    const std::string& id() const { return id_; }
    const std::string& eventId() const { return eventId_; }
    const std::string& description() const { return description_; }
    IncidentStatus status() const { return status_; }
    const std::string& resolution() const { return resolution_; }
    DateTime createdAt() const { return createdAt_; }
    void setStatus(IncidentStatus v) { status_ = v; }
    void setResolution(std::string v) { resolution_ = std::move(v); }
    nlohmann::json toJson() const;
    static Incident fromJson(const nlohmann::json& j);

private:
    std::string id_, eventId_, description_, resolution_;
    IncidentStatus status_ = IncidentStatus::Mo;
    DateTime createdAt_;
};

class AuditLog {
public:
    AuditLog() = default;
    AuditLog(std::string id, DateTime at, std::string username, std::string action, std::string detail);
    const std::string& id() const { return id_; }
    DateTime at() const { return at_; }
    const std::string& username() const { return username_; }
    const std::string& action() const { return action_; }
    const std::string& detail() const { return detail_; }
    nlohmann::json toJson() const;
    static AuditLog fromJson(const nlohmann::json& j);

private:
    std::string id_, username_, action_, detail_;
    DateTime at_;
};

struct Database {
    std::vector<User> users;
    std::vector<Customer> customers;
    std::vector<EventRequest> requests;
    std::vector<Center> centers;
    std::vector<Hall> halls;
    std::vector<ServiceItem> services;
    std::vector<Quotation> quotations;
    std::vector<Booking> bookings;
    std::vector<Contract> contracts;
    std::vector<Payment> payments;
    std::vector<Event> events;
    std::vector<OperationTask> tasks;
    std::vector<Incident> incidents;
    std::vector<AuditLog> logs;
};

}  // namespace tdp
