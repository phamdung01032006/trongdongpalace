#include "models/Enums.hpp"

using namespace std;

namespace tdp {

string toString(Role v) {
    switch (v) {
        case Role::Customer: return "Khách hàng";
        case Role::Sales: return "Nhân viên kinh doanh";
        case Role::Manager: return "Quản lý";
        case Role::Operation: return "Vận hành";
    }
    return "Khách hàng";
}

string toString(EventType v) {
    switch (v) {
        case EventType::TiecCuoi: return "Tiệc cưới";
        case EventType::SinhNhat: return "Tiệc sinh nhật";
        case EventType::GalaDinner: return "Gala Dinner";
        case EventType::HoiNghiKhachHang: return "Hội nghị khách hàng";
        case EventType::HoiThao: return "Hội thảo / Seminar";
        case EventType::RaMatSanPham: return "Ra mắt sản phẩm";
        case EventType::KyNiemThanhLap: return "Kỷ niệm thành lập";
        case EventType::TiecTatNien: return "Tiệc tất niên";
        case EventType::LeTotNghiep: return "Lễ tốt nghiệp";
        case EventType::VanHoa: return "Sự kiện văn hóa";
    }
    return "Tiệc cưới";
}

string toString(RequestStatus v) {
    switch (v) {
        case RequestStatus::Cho: return "Chờ";
        case RequestStatus::DangTuVan: return "Đang tư vấn";
        case RequestStatus::HoanThanh: return "Hoàn thành";
        case RequestStatus::BiHuy: return "Bị hủy";
        case RequestStatus::DaChot: return "Đã chốt";
    }
    return "Chờ";
}

string toString(BookingStatus v) {
    switch (v) {
        case BookingStatus::ChoCoc: return "Đang chờ đặt cọc";
        case BookingStatus::DaCoc: return "Đã đặt cọc";
        case BookingStatus::HoanTat: return "Hoàn tất";
        case BookingStatus::DaHuy: return "Bị hủy";
    }
    return "Đang chờ đặt cọc";
}

string toString(HallStatus v) {
    switch (v) {
        case HallStatus::KhaDung: return "Khả dụng";
        case HallStatus::BaoTri: return "Bảo trì";
        case HallStatus::KhongKhaDung: return "Không khả dụng";
    }
    return "Khả dụng";
}

string toString(PaymentType v) {
    switch (v) {
        case PaymentType::DatCoc1: return "Đặt cọc lần 1";
        case PaymentType::DatCoc2: return "Đặt cọc lần 2";
        case PaymentType::TatToan: return "Tất toán";
    }
    return "Đặt cọc lần 1";
}

string toString(PaymentStatus v) {
    switch (v) {
        case PaymentStatus::ChuaThanhToan: return "Chưa thanh toán";
        case PaymentStatus::DaThanhToan: return "Đã thanh toán";
    }
    return "Chưa thanh toán";
}

string toString(EventStatus v) {
    switch (v) {
        case EventStatus::SapDienRa: return "Sắp diễn ra";
        case EventStatus::DangDienRa: return "Đang diễn ra";
        case EventStatus::DaKetThuc: return "Đã kết thúc";
        case EventStatus::Huy: return "Đã hủy";
    }
    return "Sắp diễn ra";
}

string toString(PaymentMethod v) {
    switch (v) {
        case PaymentMethod::TienMat: return "Tiền mặt";
        case PaymentMethod::ChuyenKhoan: return "Chuyển khoản";
        case PaymentMethod::The: return "Thẻ tín dụng";
    }
    return "Chuyển khoản";
}

Role roleFromString(const string& s) {
    if (s == "Khách hàng" || s == "Customer") return Role::Customer;
    if (s == "Nhân viên kinh doanh" || s == "Sales") return Role::Sales;
    if (s == "Quản lý" || s == "Manager" || s == "Admin") return Role::Manager;
    if (s == "Vận hành" || s == "Operation" || s == "Coordinator") return Role::Operation;
    if (s == "Accountant") return Role::Sales;
    return Role::Customer;
}

EventType eventTypeFromString(const string& s) {
    if (s == "Tiệc sinh nhật" || s == "SinhNhat") return EventType::SinhNhat;
    if (s == "Gala Dinner") return EventType::GalaDinner;
    if (s == "Hội nghị khách hàng" || s == "HoiNghi") return EventType::HoiNghiKhachHang;
    if (s == "Hội thảo / Seminar" || s == "SuKienDoanhNghiep") return EventType::HoiThao;
    if (s == "Ra mắt sản phẩm") return EventType::RaMatSanPham;
    if (s == "Kỷ niệm thành lập") return EventType::KyNiemThanhLap;
    if (s == "Tiệc tất niên") return EventType::TiecTatNien;
    if (s == "Lễ tốt nghiệp") return EventType::LeTotNghiep;
    if (s == "Sự kiện văn hóa" || s == "Khac") return EventType::VanHoa;
    return EventType::TiecCuoi;
}

RequestStatus requestStatusFromString(const string& s) {
    if (s == "Đang tư vấn" || s == "DangTuVan") return RequestStatus::DangTuVan;
    if (s == "Hoàn thành" || s == "HoanThanh" || s == "DaBaoGia") return RequestStatus::HoanThanh;
    if (s == "Bị hủy" || s == "Huy") return RequestStatus::BiHuy;
    if (s == "Đã chốt" || s == "DaChot" || s == "DaChuyenBooking") return RequestStatus::DaChot;
    if (s == "Từ chối" || s == "TuChoi") return RequestStatus::BiHuy;
    return RequestStatus::Cho;
}

BookingStatus bookingStatusFromString(const string& s) {
    if (s == "Đã đặt cọc" || s == "DaCoc" || s == "DaXacNhan") return BookingStatus::DaCoc;
    if (s == "Hoàn tất" || s == "HoanTat") return BookingStatus::HoanTat;
    if (s == "Bị hủy" || s == "DaHuy") return BookingStatus::DaHuy;
    return BookingStatus::ChoCoc;
}

HallStatus hallStatusFromString(const string& s) {
    if (s == "Bảo trì" || s == "BaoTri") return HallStatus::BaoTri;
    if (s == "Không khả dụng" || s == "KhongKhaDung") return HallStatus::KhongKhaDung;
    return HallStatus::KhaDung;
}

PaymentType paymentTypeFromString(const string& s) {
    if (s == "Đặt cọc lần 2" || s == "DatCoc2") return PaymentType::DatCoc2;
    if (s == "Tất toán" || s == "TatToan") return PaymentType::TatToan;
    return PaymentType::DatCoc1;
}

PaymentStatus paymentStatusFromString(const string& s) {
    if (s == "Đã thanh toán" || s == "DaThanhToan") return PaymentStatus::DaThanhToan;
    return PaymentStatus::ChuaThanhToan;
}

EventStatus eventStatusFromString(const string& s) {
    if (s == "Đang diễn ra" || s == "DangDienRa") return EventStatus::DangDienRa;
    if (s == "Đã kết thúc" || s == "DaKetThuc" || s == "HoanTat") return EventStatus::DaKetThuc;
    if (s == "Đã hủy" || s == "Huy") return EventStatus::Huy;
    return EventStatus::SapDienRa;
}

PaymentMethod paymentMethodFromString(const string& s) {
    if (s == "Tiền mặt" || s == "TienMat") return PaymentMethod::TienMat;
    if (s == "Thẻ tín dụng" || s == "The") return PaymentMethod::The;
    return PaymentMethod::ChuyenKhoan;
}

string customerViewOfRequest(RequestStatus v) {
    switch (v) {
        case RequestStatus::Cho:
        case RequestStatus::DangTuVan: return "Đang xét duyệt";
        case RequestStatus::HoanThanh:
        case RequestStatus::DaChot: return "Hoàn thành";
        case RequestStatus::BiHuy: return "Bị hủy";
    }
    return "Đang xét duyệt";
}

EventTypeMeta eventTypeMeta(EventType v) {
    switch (v) {
        case EventType::TiecCuoi: return {"EVT01", "Tiệc cưới", "100-300 khách", "50-250 triệu VNĐ"};
        case EventType::SinhNhat: return {"EVT02", "Tiệc sinh nhật", "20-100 khách", "10-60 triệu VNĐ"};
        case EventType::GalaDinner: return {"EVT03", "Gala Dinner", "100-300 khách", "40-180 triệu VNĐ"};
        case EventType::HoiNghiKhachHang: return {"EVT04", "Hội nghị khách hàng", "50-200 khách", "20-100 triệu VNĐ"};
        case EventType::HoiThao: return {"EVT05", "Hội thảo / Seminar", "30-150 khách", "10-70 triệu VNĐ"};
        case EventType::RaMatSanPham: return {"EVT06", "Ra mắt sản phẩm", "100-300 khách", "50-300 triệu VNĐ"};
        case EventType::KyNiemThanhLap: return {"EVT07", "Kỷ niệm thành lập", "100-300 khách", "40-200 triệu VNĐ"};
        case EventType::TiecTatNien: return {"EVT08", "Tiệc tất niên", "100-500 khách", "40-250 triệu VNĐ"};
        case EventType::LeTotNghiep: return {"EVT09", "Lễ tốt nghiệp", "100-500 khách", "20-120 triệu VNĐ"};
        case EventType::VanHoa: return {"EVT10", "Sự kiện văn hóa", "100-300 khách", "30-200 triệu VNĐ"};
    }
    return {"EVT01", "Tiệc cưới", "100-300 khách", "50-250 triệu VNĐ"};
}

EventStatus eventTimeStatus(DateTime start, DateTime end, DateTime now) {
    if (now < start) return EventStatus::SapDienRa;
    if (end < now) return EventStatus::DaKetThuc;
    return EventStatus::DangDienRa;
}

nlohmann::json dateToJson(const DateTime& d) { return d.toString(); }

DateTime dateFromJson(const nlohmann::json& j) {
    auto parsed = DateTime::parse(j.get<string>());
    return parsed.value_or(DateTime{});
}

}  // namespace tdp
