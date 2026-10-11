#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

// =====================================================================
// Tập hợp các lớp ngoại lệ tùy chỉnh của hệ thống (Mục 7, FR-11).
//
// Nguyên lý KẾ THỪA được áp dụng tại đây:
//   BaseException kế thừa std::exception và cài đặt lại what();
//   8 lớp exception cụ thể kế thừa BaseException, chỉ cần truyền
//   thông điệp lỗi vào hàm dựng mà không phải viết lại what().
// =====================================================================
class BaseException : public exception {
protected:
    string thongDiep;
public:
    explicit BaseException(const string& msg) : thongDiep(msg) {}
    const char* what() const noexcept override { return thongDiep.c_str(); }
};

// Số khách vượt sức chứa sảnh, hoặc nhập sức chứa <= 0
class InvalidCapacityException : public BaseException {
public:
    explicit InvalidCapacityException(const string& msg) : BaseException(msg) {}
};

// Ngày/giờ nhập sai định dạng hoặc là thời điểm trong quá khứ
class InvalidDateException : public BaseException {
public:
    explicit InvalidDateException(const string& msg) : BaseException(msg) {}
};

// Mã sảnh không tồn tại
class HallNotFoundException : public BaseException {
public:
    explicit HallNotFoundException(const string& msg) : BaseException(msg) {}
};

// Khung giờ (đã cộng chuẩn bị/dọn dẹp) trùng với booking khác
class DoubleBookingException : public BaseException {
public:
    explicit DoubleBookingException(const string& msg) : BaseException(msg) {}
};

// Vi phạm điều kiện riêng của loại sự kiện (VD: tiệc cưới < 50 khách)
class EventConditionViolationException : public BaseException {
public:
    explicit EventConditionViolationException(const string& msg) : BaseException(msg) {}
};

// Hủy lịch trễ hơn thời hạn quy định (48 giờ)
class LateCancellationException : public BaseException {
public:
    explicit LateCancellationException(const string& msg) : BaseException(msg) {}
};

// Thao tác không hợp lệ với trạng thái hiện tại của booking
class InvalidStateTransitionException : public BaseException {
public:
    explicit InvalidStateTransitionException(const string& msg) : BaseException(msg) {}
};

// Cố xóa sảnh đang có booking Approved trong tương lai
class HallInUseException : public BaseException {
public:
    explicit HallInUseException(const string& msg) : BaseException(msg) {}
};

#endif // EXCEPTIONS_H
