#include "Enums.h"

#include "Utils.h"

using namespace std;

string toString(Role value) {
    switch (value) {
        case Role::Customer: return "Khách hàng";
        case Role::Sales: return "Nhân viên kinh doanh";
        case Role::Manager: return "Quản lý";
        case Role::Operation: return "Vận hành";
    }
    return "Khách hàng";
}

string toString(EventType value) {
    switch (value) {
        case EventType::TiecCuoi: return "Tiệc cưới";
        case EventType::SinhNhat: return "Tiệc sinh nhật";
        case EventType::GalaDinner: return "Gala Dinner";
        case EventType::HoiNghi: return "Hội nghị khách hàng";
        case EventType::HoiThao: return "Hội thảo / Seminar";
        case EventType::RaMat: return "Ra mắt sản phẩm";
        case EventType::KyNiem: return "Kỷ niệm thành lập";
        case EventType::TatNien: return "Tiệc tất niên";
        case EventType::TotNghiep: return "Lễ tốt nghiệp";
        case EventType::VanHoa: return "Sự kiện văn hóa";
    }
    return "Tiệc cưới";
}

string toString(RequestStatus value) {
    switch (value) {
        case RequestStatus::Cho: return "Chờ";
        case RequestStatus::DangTuVan: return "Đang tư vấn";
        case RequestStatus::HoanThanh: return "Hoàn thành";
        case RequestStatus::DaChot: return "Đã chốt";
        case RequestStatus::BiHuy: return "Bị hủy";
    }
    return "Chờ";
}

string toString(BookingStatus value) {
    switch (value) {
        case BookingStatus::ChoCoc: return "Đang chờ đặt cọc";
        case BookingStatus::DaCoc: return "Đã đặt cọc";
        case BookingStatus::HoanTat: return "Hoàn tất";
        case BookingStatus::DaHuy: return "Bị hủy";
    }
    return "Đang chờ đặt cọc";
}

string toString(HallStatus value) {
    switch (value) {
        case HallStatus::KhaDung: return "Khả dụng";
        case HallStatus::BaoTri: return "Bảo trì";
        case HallStatus::KhongKhaDung: return "Không khả dụng";
    }
    return "Khả dụng";
}

string toString(PaymentType value) {
    switch (value) {
        case PaymentType::DatCoc1: return "Đặt cọc lần 1";
        case PaymentType::DatCoc2: return "Đặt cọc lần 2";
        case PaymentType::TatToan: return "Tất toán";
    }
    return "Đặt cọc lần 1";
}

string toString(PaymentStatus value) {
    switch (value) {
        case PaymentStatus::ChuaThanhToan: return "Chưa thanh toán";
        case PaymentStatus::DaThanhToan: return "Đã thanh toán";
    }
    return "Chưa thanh toán";
}

string toString(PaymentMethod value) {
    switch (value) {
        case PaymentMethod::TienMat: return "Tiền mặt";
        case PaymentMethod::ChuyenKhoan: return "Chuyển khoản";
        case PaymentMethod::The: return "Thẻ tín dụng";
    }
    return "Chuyển khoản";
}

string toString(EventStatus value) {
    switch (value) {
        case EventStatus::SapDienRa: return "Sắp diễn ra";
        case EventStatus::DangDienRa: return "Đang diễn ra";
        case EventStatus::DaKetThuc: return "Đã kết thúc";
        case EventStatus::DaHuy: return "Đã hủy";
    }
    return "Sắp diễn ra";
}

Role roleFromString(const string& text) {
    if (text == "Khách hàng" || text == "Customer") return Role::Customer;
    if (text == "Nhân viên kinh doanh" || text == "Sales") return Role::Sales;
    if (text == "Quản lý" || text == "Manager" || text == "Admin") return Role::Manager;
    if (text == "Vận hành" || text == "Operation") return Role::Operation;
    return Role::Customer;
}

EventType eventTypeFromString(const string& text) {
    if (text == "Tiệc sinh nhật" || text == "SinhNhat") return EventType::SinhNhat;
    if (text == "Gala Dinner" || text == "GalaDinner") return EventType::GalaDinner;
    if (text == "Hội nghị khách hàng" || text == "HoiNghi") return EventType::HoiNghi;
    if (text == "Hội thảo / Seminar" || text == "HoiThao") return EventType::HoiThao;
    if (text == "Ra mắt sản phẩm" || text == "RaMat") return EventType::RaMat;
    if (text == "Kỷ niệm thành lập" || text == "KyNiem") return EventType::KyNiem;
    if (text == "Tiệc tất niên" || text == "TatNien") return EventType::TatNien;
    if (text == "Lễ tốt nghiệp" || text == "TotNghiep") return EventType::TotNghiep;
    if (text == "Sự kiện văn hóa" || text == "VanHoa") return EventType::VanHoa;
    return EventType::TiecCuoi;
}

RequestStatus requestStatusFromString(const string& text) {
    if (text == "Đang tư vấn" || text == "DangTuVan") return RequestStatus::DangTuVan;
    if (text == "Hoàn thành" || text == "HoanThanh") return RequestStatus::HoanThanh;
    if (text == "Đã chốt" || text == "DaChot") return RequestStatus::DaChot;
    if (text == "Bị hủy" || text == "BiHuy" || text == "Từ chối") return RequestStatus::BiHuy;
    return RequestStatus::Cho;
}

BookingStatus bookingStatusFromString(const string& text) {
    if (text == "Đã đặt cọc" || text == "DaCoc") return BookingStatus::DaCoc;
    if (text == "Hoàn tất" || text == "HoanTat") return BookingStatus::HoanTat;
    if (text == "Bị hủy" || text == "DaHuy") return BookingStatus::DaHuy;
    return BookingStatus::ChoCoc;
}

HallStatus hallStatusFromString(const string& text) {
    if (text == "Bảo trì" || text == "BaoTri") return HallStatus::BaoTri;
    if (text == "Không khả dụng" || text == "KhongKhaDung") return HallStatus::KhongKhaDung;
    return HallStatus::KhaDung;
}

PaymentType paymentTypeFromString(const string& text) {
    if (text == "Đặt cọc lần 2" || text == "DatCoc2") return PaymentType::DatCoc2;
    if (text == "Tất toán" || text == "TatToan") return PaymentType::TatToan;
    return PaymentType::DatCoc1;
}

PaymentStatus paymentStatusFromString(const string& text) {
    if (text == "Đã thanh toán" || text == "DaThanhToan") return PaymentStatus::DaThanhToan;
    return PaymentStatus::ChuaThanhToan;
}

PaymentMethod paymentMethodFromString(const string& text) {
    if (text == "Tiền mặt" || text == "TienMat") return PaymentMethod::TienMat;
    if (text == "Thẻ tín dụng" || text == "The") return PaymentMethod::The;
    return PaymentMethod::ChuyenKhoan;
}

EventStatus eventStatusFromString(const string& text) {
    if (text == "Đang diễn ra" || text == "DangDienRa") return EventStatus::DangDienRa;
    if (text == "Đã kết thúc" || text == "DaKetThuc") return EventStatus::DaKetThuc;
    if (text == "Đã hủy" || text == "DaHuy") return EventStatus::DaHuy;
    return EventStatus::SapDienRa;
}

EventTypeInfo eventTypeInfo(EventType value) {
    switch (value) {
        case EventType::TiecCuoi: return {"EVT01", "Tiệc cưới", "100-300 khách", "50-250 triệu"};
        case EventType::SinhNhat: return {"EVT02", "Tiệc sinh nhật", "20-100 khách", "10-60 triệu"};
        case EventType::GalaDinner: return {"EVT03", "Gala Dinner", "100-300 khách", "40-180 triệu"};
        case EventType::HoiNghi: return {"EVT04", "Hội nghị khách hàng", "50-200 khách", "20-100 triệu"};
        case EventType::HoiThao: return {"EVT05", "Hội thảo / Seminar", "30-150 khách", "10-70 triệu"};
        case EventType::RaMat: return {"EVT06", "Ra mắt sản phẩm", "100-300 khách", "50-300 triệu"};
        case EventType::KyNiem: return {"EVT07", "Kỷ niệm thành lập", "100-300 khách", "40-200 triệu"};
        case EventType::TatNien: return {"EVT08", "Tiệc tất niên", "100-500 khách", "40-250 triệu"};
        case EventType::TotNghiep: return {"EVT09", "Lễ tốt nghiệp", "100-500 khách", "20-120 triệu"};
        case EventType::VanHoa: return {"EVT10", "Sự kiện văn hóa", "100-300 khách", "30-200 triệu"};
    }
    return {"EVT01", "Tiệc cưới", "100-300 khách", "50-250 triệu"};
}

string customerViewOfRequest(RequestStatus value) {
    switch (value) {
        case RequestStatus::Cho:
        case RequestStatus::DangTuVan:
            return "Đang xét duyệt";
        case RequestStatus::HoanThanh:
        case RequestStatus::DaChot:
            return "Hoàn thành";
        case RequestStatus::BiHuy:
            return "Bị hủy";
    }
    return "Đang xét duyệt";
}

EventStatus eventTimeStatus(const DateTime& start, const DateTime& end, const DateTime& now) {
    if (now < start) return EventStatus::SapDienRa;
    if (end < now) return EventStatus::DaKetThuc;
    return EventStatus::DangDienRa;
}
