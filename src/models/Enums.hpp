#pragma once

#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

// Vai tro theo dac ta muc 1: Khach hang / Sales / Quan ly / Van hanh.
enum class Role { Customer, Sales, Manager, Operation };
// 10 loai su kien theo dac ta 2.1 (EVT01..EVT10).
enum class EventType {
    TiecCuoi,
    SinhNhat,
    GalaDinner,
    HoiNghiKhachHang,
    HoiThao,
    RaMatSanPham,
    KyNiemThanhLap,
    TiecTatNien,
    LeTotNghiep,
    VanHoa
};
// Trang thai yeu cau (goc nhin sales). Khach hang chi thay anh xa 2 lop
// (customerViewOfRequest): Cho/DangTuVan -> "Dang xet duyet".
enum class RequestStatus { Cho, DangTuVan, HoanThanh, BiHuy, DaChot };
// Trang thai booking theo dac ta B.2 + Form 2.
enum class BookingStatus { ChoCoc, DaCoc, HoanTat, DaHuy };
// Trang thai sanh theo dac ta C: Kha dung / Bao tri / Khong kha dung.
enum class HallStatus { KhaDung, BaoTri, KhongKhaDung };
// Loai thanh toan theo Form 3.
enum class PaymentType { DatCoc1, DatCoc2, TatToan };
enum class PaymentStatus { ChuaThanhToan, DaThanhToan };
// Trang thai su kien theo thoi gian (dac ta D).
enum class EventStatus { SapDienRa, DangDienRa, DaKetThuc, Huy };
enum class PaymentMethod { TienMat, ChuyenKhoan, The };

struct EventTypeMeta {
    const char* code;
    const char* name;
    const char* scale;
    const char* budget;
};

string toString(Role v);
string toString(EventType v);
string toString(RequestStatus v);
string toString(BookingStatus v);
string toString(HallStatus v);
string toString(PaymentType v);
string toString(PaymentStatus v);
string toString(EventStatus v);
string toString(PaymentMethod v);

Role roleFromString(const string& s);
EventType eventTypeFromString(const string& s);
RequestStatus requestStatusFromString(const string& s);
BookingStatus bookingStatusFromString(const string& s);
HallStatus hallStatusFromString(const string& s);
PaymentType paymentTypeFromString(const string& s);
PaymentStatus paymentStatusFromString(const string& s);
EventStatus eventStatusFromString(const string& s);
PaymentMethod paymentMethodFromString(const string& s);

// Anh xa 2 lop trang thai yeu cau sang goc nhin khach hang (dac ta A.1).
string customerViewOfRequest(RequestStatus v);
EventTypeMeta eventTypeMeta(EventType v);
// Trang thai su kien tinh theo thoi gian thuc (dac ta D).
EventStatus eventTimeStatus(DateTime start, DateTime end, DateTime now);

nlohmann::json dateToJson(const DateTime& d);
DateTime dateFromJson(const nlohmann::json& j);

}  // namespace tdp
