#pragma once

// Cac kieu liet ke (enum) dung trong toan bo he thong.
// Moi enum deu co 2 chieu: toString (de in ra man hinh / ghi file)
// va ...FromString (de doc lai tu file text).

#include "DateTime.h"

#include <string>

using namespace std;

enum class Role { Customer, Sales, Manager, Operation };

enum class EventType {
    TiecCuoi,
    SinhNhat,
    GalaDinner,
    HoiNghi,
    HoiThao,
    RaMat,
    KyNiem,
    TatNien,
    TotNghiep,
    VanHoa
};

enum class RequestStatus { Cho, DangTuVan, HoanThanh, DaChot, BiHuy };
enum class BookingStatus { ChoCoc, DaCoc, HoanTat, DaHuy };
enum class HallStatus { KhaDung, BaoTri, KhongKhaDung };
enum class PaymentType { DatCoc1, DatCoc2, TatToan };
enum class PaymentStatus { ChuaThanhToan, DaThanhToan };
enum class PaymentMethod { TienMat, ChuyenKhoan, The };
enum class EventStatus { SapDienRa, DangDienRa, DaKetThuc, DaHuy };

// Thong tin mo ta mot loai su kien (ma, ten, quy mo, ngan sach).
struct EventTypeInfo {
    const char* code;
    const char* name;
    const char* scale;
    const char* budget;
};

string toString(Role value);
string toString(EventType value);
string toString(RequestStatus value);
string toString(BookingStatus value);
string toString(HallStatus value);
string toString(PaymentType value);
string toString(PaymentStatus value);
string toString(PaymentMethod value);
string toString(EventStatus value);

Role roleFromString(const string& text);
EventType eventTypeFromString(const string& text);
RequestStatus requestStatusFromString(const string& text);
BookingStatus bookingStatusFromString(const string& text);
HallStatus hallStatusFromString(const string& text);
PaymentType paymentTypeFromString(const string& text);
PaymentStatus paymentStatusFromString(const string& text);
PaymentMethod paymentMethodFromString(const string& text);
EventStatus eventStatusFromString(const string& text);

EventTypeInfo eventTypeInfo(EventType value);

// Khach hang chi nhin thay 3 trang thai don gian thay vi 5 trang thai noi bo.
string customerViewOfRequest(RequestStatus value);

// Trang thai su kien duoc tinh tu dong theo dong ho may tinh.
EventStatus eventTimeStatus(const DateTime& start, const DateTime& end, const DateTime& now);
