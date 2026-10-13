#pragma once

#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

enum class Role { Admin, Sales, Accountant, Coordinator };
enum class EventType { TiecCuoi, HoiNghi, SinhNhat, SuKienDoanhNghiep, Khac };
enum class RequestStatus { TiepNhan, DaBaoGia, DaChuyenBooking, Huy, TuChoi };
enum class BookingStatus { Nhat, DaXacNhan, DaHuy };
enum class EventStatus { ChuanBi, DangDienRa, HoanTat, Huy };
enum class PaymentMethod { TienMat, ChuyenKhoan, The };

string toString(Role v);
string toString(EventType v);
string toString(RequestStatus v);
string toString(BookingStatus v);
string toString(EventStatus v);
string toString(PaymentMethod v);

Role roleFromString(const string& s);
EventType eventTypeFromString(const string& s);
RequestStatus requestStatusFromString(const string& s);
BookingStatus bookingStatusFromString(const string& s);
EventStatus eventStatusFromString(const string& s);
PaymentMethod paymentMethodFromString(const string& s);

nlohmann::json dateToJson(const DateTime& d);
DateTime dateFromJson(const nlohmann::json& j);

}  // namespace tdp
