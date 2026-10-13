#pragma once

#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

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

string toString(Role v);
string toString(EventType v);
string toString(RequestStatus v);
string toString(QuotationStatus v);
string toString(BookingStatus v);
string toString(ContractStatus v);
string toString(EventStatus v);
string toString(TaskStatus v);
string toString(IncidentStatus v);
string toString(PaymentMethod v);

Role roleFromString(const string& s);
EventType eventTypeFromString(const string& s);
RequestStatus requestStatusFromString(const string& s);
QuotationStatus quotationStatusFromString(const string& s);
BookingStatus bookingStatusFromString(const string& s);
ContractStatus contractStatusFromString(const string& s);
EventStatus eventStatusFromString(const string& s);
TaskStatus taskStatusFromString(const string& s);
IncidentStatus incidentStatusFromString(const string& s);
PaymentMethod paymentMethodFromString(const string& s);

nlohmann::json dateToJson(const DateTime& d);
DateTime dateFromJson(const nlohmann::json& j);

}  // namespace tdp
