#include "models/Enums.hpp"

namespace tdp {

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

}  // namespace tdp
