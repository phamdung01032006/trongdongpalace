#ifndef BOOKINGREQUEST_H
#define BOOKINGREQUEST_H

#include <string>
#include "Utils.h"
#include "Hall.h"
#include "Event.h"

using namespace std;

// =====================================================================
// Trạng thái của một yêu cầu đặt lịch — vòng đời theo Mục 8:
//   [Pending] --duyệt--> [Approved] --hủy--> [Cancelled]
//   [Pending] --từ chối--> [Rejected]
//   [Approved] --đổi lịch hợp lệ--> [Approved] (khung giờ mới)
// Mọi chuyển trạng thái khác đều ném InvalidStateTransitionException.
// =====================================================================
enum TrangThai {
    PENDING,   // đang chờ duyệt
    APPROVED,  // đã duyệt
    REJECTED,  // đã từ chối
    CANCELLED  // đã hủy
};

string trangThaiThanhChu(TrangThai t);

// =====================================================================
// Lớp cụ thể: một yêu cầu đặt lịch sử dụng sảnh (Mục 6).
// Quan hệ has-a: BookingRequest HAS-A Hall* và HAS-A Event*
// (con trỏ đa hình — cho phép mỗi booking gắn với bất kỳ loại
// sự kiện nào kế thừa từ Event mà không cần biết loại cụ thể).
// =====================================================================
class BookingRequest {
private:
    string maBooking;
    Hall* sanh;        // has-a: booking thuộc một sảnh
    Event* suKien;     // has-a: booking gắn với một đối tượng sự kiện (đa hình Event*)
    TrangThai trangThai;
    ThoiDiem ngayTao;  // thời điểm tạo yêu cầu
    string lyDoTuChoi; // lý do từ chối (FR-07), rỗng nếu không bị từ chối

public:
    BookingRequest(const string& maBooking, Hall* sanh, Event* suKien);
    ~BookingRequest();

    string getMaBooking() const;
    TrangThai getTrangThai() const;
    Hall* getSanh() const;
    Event* getSuKien() const;
    ThoiDiem getNgayTao() const;
    string getLyDoTuChoi() const;

    void setTrangThai(TrangThai trangThaiMoi);
    void setLyDoTuChoi(const string& lyDo);
    void setNgayTao(const ThoiDiem& ngayTaoMoi);

    void hienThi() const;
};

#endif // BOOKINGREQUEST_H
