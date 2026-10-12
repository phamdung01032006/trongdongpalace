#pragma once

#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

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

}  // namespace tdp
