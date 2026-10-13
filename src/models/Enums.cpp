#include "models/Enums.hpp"

using namespace std;

namespace tdp {

string toString(Role v) {
    switch (v) {
        case Role::Admin: return "Admin";
        case Role::Sales: return "Sales";
        case Role::Accountant: return "Accountant";
        case Role::Coordinator: return "Coordinator";
    }
    return "Sales";
}

string toString(EventType v) {
    switch (v) {
        case EventType::TiecCuoi: return "TiecCuoi";
        case EventType::HoiNghi: return "HoiNghi";
        case EventType::SinhNhat: return "SinhNhat";
        case EventType::SuKienDoanhNghiep: return "SuKienDoanhNghiep";
        case EventType::Khac: return "Khac";
    }
    return "Khac";
}

string toString(RequestStatus v) {
    switch (v) {
        case RequestStatus::TiepNhan: return "TiepNhan";
        case RequestStatus::DaBaoGia: return "DaBaoGia";
        case RequestStatus::DaChuyenBooking: return "DaChuyenBooking";
        case RequestStatus::Huy: return "Huy";
        case RequestStatus::TuChoi: return "TuChoi";
    }
    return "TiepNhan";
}

string toString(BookingStatus v) {
    switch (v) {
        case BookingStatus::Nhat: return "Nhat";
        case BookingStatus::DaXacNhan: return "DaXacNhan";
        case BookingStatus::DaHuy: return "DaHuy";
    }
    return "Nhat";
}

string toString(EventStatus v) {
    switch (v) {
        case EventStatus::ChuanBi: return "ChuanBi";
        case EventStatus::DangDienRa: return "DangDienRa";
        case EventStatus::HoanTat: return "HoanTat";
        case EventStatus::Huy: return "Huy";
    }
    return "ChuanBi";
}

string toString(PaymentMethod v) {
    switch (v) {
        case PaymentMethod::TienMat: return "TienMat";
        case PaymentMethod::ChuyenKhoan: return "ChuyenKhoan";
        case PaymentMethod::The: return "The";
    }
    return "ChuyenKhoan";
}

Role roleFromString(const string& s) {
    if (s == "Admin") return Role::Admin;
    if (s == "Accountant") return Role::Accountant;
    if (s == "Coordinator") return Role::Coordinator;
    return Role::Sales;
}

EventType eventTypeFromString(const string& s) {
    if (s == "HoiNghi") return EventType::HoiNghi;
    if (s == "SinhNhat") return EventType::SinhNhat;
    if (s == "SuKienDoanhNghiep") return EventType::SuKienDoanhNghiep;
    if (s == "Khac") return EventType::Khac;
    return EventType::TiecCuoi;
}

RequestStatus requestStatusFromString(const string& s) {
    if (s == "DaBaoGia") return RequestStatus::DaBaoGia;
    if (s == "DaChuyenBooking") return RequestStatus::DaChuyenBooking;
    if (s == "Huy") return RequestStatus::Huy;
    if (s == "TuChoi") return RequestStatus::TuChoi;
    return RequestStatus::TiepNhan;
}

BookingStatus bookingStatusFromString(const string& s) {
    if (s == "DaXacNhan") return BookingStatus::DaXacNhan;
    if (s == "DaHuy") return BookingStatus::DaHuy;
    return BookingStatus::Nhat;
}

EventStatus eventStatusFromString(const string& s) {
    if (s == "DangDienRa") return EventStatus::DangDienRa;
    if (s == "HoanTat") return EventStatus::HoanTat;
    if (s == "Huy") return EventStatus::Huy;
    return EventStatus::ChuanBi;
}

PaymentMethod paymentMethodFromString(const string& s) {
    if (s == "TienMat") return PaymentMethod::TienMat;
    if (s == "The") return PaymentMethod::The;
    return PaymentMethod::ChuyenKhoan;
}

nlohmann::json dateToJson(const DateTime& d) { return d.toString(); }

DateTime dateFromJson(const nlohmann::json& j) {
    auto parsed = DateTime::parse(j.get<string>());
    return parsed.value_or(DateTime{});
}

}  // namespace tdp
