#include "models/entities.hpp"

namespace tdp {
namespace {

template <typename T>
T parseEnum(const std::string& s, T (*fn)(const std::string&)) {
    return fn(s);
}

}  // namespace

std::string toString(Role v) {
    switch (v) {
        case Role::Admin: return "Admin";
        case Role::Sales: return "Sales";
        case Role::Accountant: return "Accountant";
        case Role::Coordinator: return "Coordinator";
    }
    return "Sales";
}
std::string toString(EventType v) {
    switch (v) {
        case EventType::TiecCuoi: return "TiecCuoi";
        case EventType::HoiNghi: return "HoiNghi";
        case EventType::SinhNhat: return "SinhNhat";
        case EventType::SuKienDoanhNghiep: return "SuKienDoanhNghiep";
        case EventType::Khac: return "Khac";
    }
    return "Khac";
}
std::string toString(RequestStatus v) {
    switch (v) {
        case RequestStatus::TiepNhan: return "TiepNhan";
        case RequestStatus::DaBaoGia: return "DaBaoGia";
        case RequestStatus::DaChuyenBooking: return "DaChuyenBooking";
        case RequestStatus::Huy: return "Huy";
        case RequestStatus::TuChoi: return "TuChoi";
    }
    return "TiepNhan";
}
std::string toString(QuotationStatus v) {
    switch (v) {
        case QuotationStatus::Nhat: return "Nhat";
        case QuotationStatus::DaGui: return "DaGui";
        case QuotationStatus::DaChapNhan: return "DaChapNhan";
        case QuotationStatus::TuChoi: return "TuChoi";
    }
    return "Nhat";
}
std::string toString(BookingStatus v) {
    switch (v) {
        case BookingStatus::Nhat: return "Nhat";
        case BookingStatus::DaXacNhan: return "DaXacNhan";
        case BookingStatus::DaHuy: return "DaHuy";
    }
    return "Nhat";
}
std::string toString(ContractStatus v) {
    switch (v) {
        case ContractStatus::HieuLuc: return "HieuLuc";
        case ContractStatus::HoanTat: return "HoanTat";
        case ContractStatus::Huy: return "Huy";
    }
    return "HieuLuc";
}
std::string toString(EventStatus v) {
    switch (v) {
        case EventStatus::ChuanBi: return "ChuanBi";
        case EventStatus::DangDienRa: return "DangDienRa";
        case EventStatus::HoanTat: return "HoanTat";
        case EventStatus::Huy: return "Huy";
    }
    return "ChuanBi";
}
std::string toString(TaskStatus v) {
    switch (v) {
        case TaskStatus::CanLam: return "CanLam";
        case TaskStatus::DangLam: return "DangLam";
        case TaskStatus::HoanThanh: return "HoanThanh";
    }
    return "CanLam";
}
std::string toString(IncidentStatus v) {
    switch (v) {
        case IncidentStatus::Mo: return "Mo";
        case IncidentStatus::DangXuLy: return "DangXuLy";
        case IncidentStatus::DaXuLy: return "DaXuLy";
    }
    return "Mo";
}
std::string toString(PaymentMethod v) {
    switch (v) {
        case PaymentMethod::TienMat: return "TienMat";
        case PaymentMethod::ChuyenKhoan: return "ChuyenKhoan";
        case PaymentMethod::The: return "The";
    }
    return "ChuyenKhoan";
}

Role roleFromString(const std::string& s) {
    if (s == "Admin") return Role::Admin;
    if (s == "Accountant") return Role::Accountant;
    if (s == "Coordinator") return Role::Coordinator;
    return Role::Sales;
}
EventType eventTypeFromString(const std::string& s) {
    if (s == "HoiNghi") return EventType::HoiNghi;
    if (s == "SinhNhat") return EventType::SinhNhat;
    if (s == "SuKienDoanhNghiep") return EventType::SuKienDoanhNghiep;
    if (s == "Khac") return EventType::Khac;
    return EventType::TiecCuoi;
}
RequestStatus requestStatusFromString(const std::string& s) {
    if (s == "DaBaoGia") return RequestStatus::DaBaoGia;
    if (s == "DaChuyenBooking") return RequestStatus::DaChuyenBooking;
    if (s == "Huy") return RequestStatus::Huy;
    if (s == "TuChoi") return RequestStatus::TuChoi;
    return RequestStatus::TiepNhan;
}
QuotationStatus quotationStatusFromString(const std::string& s) {
    if (s == "DaGui") return QuotationStatus::DaGui;
    if (s == "DaChapNhan") return QuotationStatus::DaChapNhan;
    if (s == "TuChoi") return QuotationStatus::TuChoi;
    return QuotationStatus::Nhat;
}
BookingStatus bookingStatusFromString(const std::string& s) {
    if (s == "DaXacNhan") return BookingStatus::DaXacNhan;
    if (s == "DaHuy") return BookingStatus::DaHuy;
    return BookingStatus::Nhat;
}
ContractStatus contractStatusFromString(const std::string& s) {
    if (s == "HoanTat") return ContractStatus::HoanTat;
    if (s == "Huy") return ContractStatus::Huy;
    return ContractStatus::HieuLuc;
}
EventStatus eventStatusFromString(const std::string& s) {
    if (s == "DangDienRa") return EventStatus::DangDienRa;
    if (s == "HoanTat") return EventStatus::HoanTat;
    if (s == "Huy") return EventStatus::Huy;
    return EventStatus::ChuanBi;
}
TaskStatus taskStatusFromString(const std::string& s) {
    if (s == "DangLam") return TaskStatus::DangLam;
    if (s == "HoanThanh") return TaskStatus::HoanThanh;
    return TaskStatus::CanLam;
}
IncidentStatus incidentStatusFromString(const std::string& s) {
    if (s == "DangXuLy") return IncidentStatus::DangXuLy;
    if (s == "DaXuLy") return IncidentStatus::DaXuLy;
    return IncidentStatus::Mo;
}
PaymentMethod paymentMethodFromString(const std::string& s) {
    if (s == "TienMat") return PaymentMethod::TienMat;
    if (s == "The") return PaymentMethod::The;
    return PaymentMethod::ChuyenKhoan;
}

nlohmann::json dateToJson(const DateTime& d) { return d.toString(); }
DateTime dateFromJson(const nlohmann::json& j) {
    auto parsed = DateTime::parse(j.get<std::string>());
    return parsed.value_or(DateTime{});
}

User::User(std::string id, std::string username, std::string salt, std::string passwordHash, std::string fullName,
           Role role, bool active)
    : id_(std::move(id)),
      username_(std::move(username)),
      salt_(std::move(salt)),
      passwordHash_(std::move(passwordHash)),
      fullName_(std::move(fullName)),
      role_(role),
      active_(active) {}

nlohmann::json User::toJson() const {
    return {{"id", id_},
            {"username", username_},
            {"salt", salt_},
            {"passwordHash", passwordHash_},
            {"demoPassword", demoPassword_},
            {"fullName", fullName_},
            {"role", toString(role_)},
            {"active", active_}};
}
User User::fromJson(const nlohmann::json& j) {
    User u(j.at("id").get<std::string>(), j.at("username").get<std::string>(), j.at("salt").get<std::string>(),
           j.at("passwordHash").get<std::string>(), j.value("fullName", ""),
           roleFromString(j.value("role", "Sales")), j.value("active", true));
    u.setDemoPassword(j.value("demoPassword", ""));
    return u;
}

Customer::Customer(std::string id, std::string name, std::string phone, std::string email, std::string address,
                   std::string notes, DateTime createdAt)
    : id_(std::move(id)),
      name_(std::move(name)),
      phone_(std::move(phone)),
      email_(std::move(email)),
      address_(std::move(address)),
      notes_(std::move(notes)),
      createdAt_(createdAt) {}

nlohmann::json Customer::toJson() const {
    return {{"id", id_}, {"name", name_}, {"phone", phone_}, {"email", email_},
            {"address", address_}, {"notes", notes_}, {"createdAt", dateToJson(createdAt_)}};
}
Customer Customer::fromJson(const nlohmann::json& j) {
    return Customer(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.at("phone").get<std::string>(),
                    j.value("email", ""), j.value("address", ""), j.value("notes", ""),
                    dateFromJson(j.at("createdAt")));
}

EventRequest::EventRequest(std::string id, std::string customerId, EventType type, std::string title, DateTime start,
                           DateTime end, int guestCount, std::string specialRequests, RequestStatus status)
    : id_(std::move(id)),
      customerId_(std::move(customerId)),
      title_(std::move(title)),
      specialRequests_(std::move(specialRequests)),
      type_(type),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      status_(status) {}

nlohmann::json EventRequest::toJson() const {
    return {{"id", id_},
            {"customerId", customerId_},
            {"type", toString(type_)},
            {"title", title_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"specialRequests", specialRequests_},
            {"status", toString(status_)}};
}
EventRequest EventRequest::fromJson(const nlohmann::json& j) {
    return EventRequest(j.at("id").get<std::string>(), j.at("customerId").get<std::string>(),
                        eventTypeFromString(j.value("type", "TiecCuoi")), j.value("title", ""),
                        dateFromJson(j.at("start")), dateFromJson(j.at("end")), j.value("guestCount", 0),
                        j.value("specialRequests", ""), requestStatusFromString(j.value("status", "TiepNhan")));
}

Center::Center(std::string id, std::string name, std::string address, bool active)
    : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)), active_(active) {}
nlohmann::json Center::toJson() const {
    return {{"id", id_}, {"name", name_}, {"address", address_}, {"active", active_}};
}
Center Center::fromJson(const nlohmann::json& j) {
    return Center(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.value("address", ""),
                  j.value("active", true));
}

Hall::Hall(std::string id, std::string centerId, std::string name, int capacity, bool active)
    : id_(std::move(id)), centerId_(std::move(centerId)), name_(std::move(name)), capacity_(capacity), active_(active) {}
nlohmann::json Hall::toJson() const {
    return {{"id", id_}, {"centerId", centerId_}, {"name", name_}, {"capacity", capacity_}, {"active", active_}};
}
Hall Hall::fromJson(const nlohmann::json& j) {
    return Hall(j.at("id").get<std::string>(), j.at("centerId").get<std::string>(), j.at("name").get<std::string>(),
                j.value("capacity", 0), j.value("active", true));
}

ServiceItem::ServiceItem(std::string id, std::string name, std::string unit, int64_t unitPrice, std::string category,
                         bool active)
    : id_(std::move(id)),
      name_(std::move(name)),
      unit_(std::move(unit)),
      category_(std::move(category)),
      unitPrice_(unitPrice),
      active_(active) {}
nlohmann::json ServiceItem::toJson() const {
    return {{"id", id_}, {"name", name_}, {"unit", unit_}, {"unitPrice", unitPrice_}, {"category", category_},
            {"active", active_}};
}
ServiceItem ServiceItem::fromJson(const nlohmann::json& j) {
    return ServiceItem(j.at("id").get<std::string>(), j.at("name").get<std::string>(), j.value("unit", "lần"),
                       j.value("unitPrice", 0), j.value("category", "Khac"), j.value("active", true));
}

QuotationLine::QuotationLine(std::string serviceId, std::string nameSnapshot, int quantity, int64_t unitPrice)
    : serviceId_(std::move(serviceId)), nameSnapshot_(std::move(nameSnapshot)), quantity_(quantity), unitPrice_(unitPrice) {}
nlohmann::json QuotationLine::toJson() const {
    return {{"serviceId", serviceId_},
            {"nameSnapshot", nameSnapshot_},
            {"quantity", quantity_},
            {"unitPrice", unitPrice_},
            {"amount", amount()}};
}
QuotationLine QuotationLine::fromJson(const nlohmann::json& j) {
    return QuotationLine(j.at("serviceId").get<std::string>(), j.value("nameSnapshot", ""), j.value("quantity", 1),
                         j.value("unitPrice", 0));
}

Quotation::Quotation(std::string id, std::string requestId, std::string customerId, std::string note)
    : id_(std::move(id)), requestId_(std::move(requestId)), customerId_(std::move(customerId)), note_(std::move(note)) {
    recalc();
}
void Quotation::recalc() {
    subtotal_ = 0;
    for (const auto& line : lines_) subtotal_ += line.amount();
    discountPercent_ = discountPercentForSubtotal(subtotal_);
    discountAmount_ = subtotal_ * discountPercent_ / 100;
    total_ = subtotal_ - discountAmount_;
}
nlohmann::json Quotation::toJson() const {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& line : lines_) arr.push_back(line.toJson());
    return {{"id", id_},
            {"requestId", requestId_},
            {"customerId", customerId_},
            {"lines", arr},
            {"discountPercent", discountPercent_},
            {"discountAmount", discountAmount_},
            {"subtotal", subtotal_},
            {"total", total_},
            {"status", toString(status_)},
            {"note", note_}};
}
Quotation Quotation::fromJson(const nlohmann::json& j) {
    Quotation q(j.at("id").get<std::string>(), j.at("requestId").get<std::string>(),
                j.at("customerId").get<std::string>(), j.value("note", ""));
    if (j.contains("lines")) {
        for (const auto& line : j["lines"]) q.lines_.push_back(QuotationLine::fromJson(line));
    }
    q.status_ = quotationStatusFromString(j.value("status", "Nhat"));
    q.recalc();
    return q;
}

Booking::Booking(std::string id, std::string quotationId, std::string requestId, std::string customerId,
                 std::string hallId, DateTime start, DateTime end, int guestCount, BookingStatus status)
    : id_(std::move(id)),
      quotationId_(std::move(quotationId)),
      requestId_(std::move(requestId)),
      customerId_(std::move(customerId)),
      hallId_(std::move(hallId)),
      start_(start),
      end_(end),
      guestCount_(guestCount),
      status_(status) {}
nlohmann::json Booking::toJson() const {
    return {{"id", id_},
            {"quotationId", quotationId_},
            {"requestId", requestId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"guestCount", guestCount_},
            {"status", toString(status_)},
            {"cancelReason", cancelReason_}};
}
Booking Booking::fromJson(const nlohmann::json& j) {
    Booking b(j.at("id").get<std::string>(), j.at("quotationId").get<std::string>(), j.at("requestId").get<std::string>(),
              j.at("customerId").get<std::string>(), j.at("hallId").get<std::string>(), dateFromJson(j.at("start")),
              dateFromJson(j.at("end")), j.value("guestCount", 0), bookingStatusFromString(j.value("status", "Nhat")));
    b.setCancelReason(j.value("cancelReason", ""));
    return b;
}

Contract::Contract(std::string id, std::string bookingId, std::string customerId, std::string contractNo,
                   DateTime signedAt, int64_t totalAmount, ContractStatus status)
    : id_(std::move(id)),
      bookingId_(std::move(bookingId)),
      customerId_(std::move(customerId)),
      contractNo_(std::move(contractNo)),
      signedAt_(signedAt),
      totalAmount_(totalAmount),
      status_(status) {}
nlohmann::json Contract::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"customerId", customerId_},
            {"contractNo", contractNo_},
            {"signedAt", dateToJson(signedAt_)},
            {"totalAmount", totalAmount_},
            {"status", toString(status_)}};
}
Contract Contract::fromJson(const nlohmann::json& j) {
    return Contract(j.at("id").get<std::string>(), j.at("bookingId").get<std::string>(),
                    j.at("customerId").get<std::string>(), j.value("contractNo", ""), dateFromJson(j.at("signedAt")),
                    j.value("totalAmount", 0), contractStatusFromString(j.value("status", "HieuLuc")));
}

Payment::Payment(std::string id, std::string contractId, int64_t amount, DateTime paidAt, PaymentMethod method,
                 std::string note, std::string recordedBy)
    : id_(std::move(id)),
      contractId_(std::move(contractId)),
      note_(std::move(note)),
      recordedBy_(std::move(recordedBy)),
      amount_(amount),
      paidAt_(paidAt),
      method_(method) {}
nlohmann::json Payment::toJson() const {
    return {{"id", id_},
            {"contractId", contractId_},
            {"amount", amount_},
            {"paidAt", dateToJson(paidAt_)},
            {"method", toString(method_)},
            {"note", note_},
            {"recordedBy", recordedBy_}};
}
Payment Payment::fromJson(const nlohmann::json& j) {
    return Payment(j.at("id").get<std::string>(), j.at("contractId").get<std::string>(), j.at("amount").get<int64_t>(),
                   dateFromJson(j.at("paidAt")), paymentMethodFromString(j.value("method", "ChuyenKhoan")),
                   j.value("note", ""), j.value("recordedBy", ""));
}

Event::Event(std::string id, std::string bookingId, std::string contractId, std::string customerId, std::string hallId,
             DateTime start, DateTime end, EventStatus status, std::string specialNotes)
    : id_(std::move(id)),
      bookingId_(std::move(bookingId)),
      contractId_(std::move(contractId)),
      customerId_(std::move(customerId)),
      hallId_(std::move(hallId)),
      specialNotes_(std::move(specialNotes)),
      start_(start),
      end_(end),
      status_(status) {}
nlohmann::json Event::toJson() const {
    return {{"id", id_},
            {"bookingId", bookingId_},
            {"contractId", contractId_},
            {"customerId", customerId_},
            {"hallId", hallId_},
            {"start", dateToJson(start_)},
            {"end", dateToJson(end_)},
            {"status", toString(status_)},
            {"specialNotes", specialNotes_}};
}
Event Event::fromJson(const nlohmann::json& j) {
    return Event(j.at("id").get<std::string>(), j.at("bookingId").get<std::string>(), j.value("contractId", ""),
                 j.at("customerId").get<std::string>(), j.at("hallId").get<std::string>(), dateFromJson(j.at("start")),
                 dateFromJson(j.at("end")), eventStatusFromString(j.value("status", "ChuanBi")),
                 j.value("specialNotes", ""));
}

OperationTask::OperationTask(std::string id, std::string eventId, std::string title, std::string assignee, DateTime due,
                             TaskStatus status, std::string note)
    : id_(std::move(id)),
      eventId_(std::move(eventId)),
      title_(std::move(title)),
      assignee_(std::move(assignee)),
      note_(std::move(note)),
      due_(due),
      status_(status) {}
nlohmann::json OperationTask::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"title", title_}, {"assignee", assignee_},
            {"due", dateToJson(due_)}, {"status", toString(status_)}, {"note", note_}};
}
OperationTask OperationTask::fromJson(const nlohmann::json& j) {
    return OperationTask(j.at("id").get<std::string>(), j.at("eventId").get<std::string>(), j.at("title").get<std::string>(),
                         j.value("assignee", ""), dateFromJson(j.at("due")),
                         taskStatusFromString(j.value("status", "CanLam")), j.value("note", ""));
}

Incident::Incident(std::string id, std::string eventId, std::string description, IncidentStatus status,
                   std::string resolution, DateTime createdAt)
    : id_(std::move(id)),
      eventId_(std::move(eventId)),
      description_(std::move(description)),
      resolution_(std::move(resolution)),
      status_(status),
      createdAt_(createdAt) {}
nlohmann::json Incident::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"description", description_}, {"status", toString(status_)},
            {"resolution", resolution_}, {"createdAt", dateToJson(createdAt_)}};
}
Incident Incident::fromJson(const nlohmann::json& j) {
    return Incident(j.at("id").get<std::string>(), j.at("eventId").get<std::string>(),
                    j.at("description").get<std::string>(), incidentStatusFromString(j.value("status", "Mo")),
                    j.value("resolution", ""), dateFromJson(j.at("createdAt")));
}

AuditLog::AuditLog(std::string id, DateTime at, std::string username, std::string action, std::string detail)
    : id_(std::move(id)), username_(std::move(username)), action_(std::move(action)), detail_(std::move(detail)), at_(at) {}
nlohmann::json AuditLog::toJson() const {
    return {{"id", id_}, {"at", dateToJson(at_)}, {"username", username_}, {"action", action_}, {"detail", detail_}};
}
AuditLog AuditLog::fromJson(const nlohmann::json& j) {
    return AuditLog(j.at("id").get<std::string>(), dateFromJson(j.at("at")), j.value("username", ""),
                    j.value("action", ""), j.value("detail", ""));
}

}  // namespace tdp
