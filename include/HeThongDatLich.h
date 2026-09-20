#ifndef HETHONGDATLICH_H
#define HETHONGDATLICH_H

#include <string>
#include <vector>
#include "Hall.h"
#include "Event.h"
#include "BookingRequest.h"
#include "Exceptions.h"

using namespace std;

// =====================================================================
// Lớp trung tâm điều phối toàn bộ nghiệp vụ của hệ thống:
//   - Quản lý danh mục sảnh (FR-01)
//   - Tạo/duyệt/từ chối/hủy/đổi lịch booking (FR-04 -> FR-09)
//   - Xem lịch trống, tra cứu (FR-03, FR-10)
// Mọi ngoại lệ nghiệp vụ đều được ném ra từ lớp này (FR-11) và được
// tầng menu (main.cpp) bắt lại để hiển thị, không để chương trình chết.
// =====================================================================
class HeThongDatLich {
private:
    vector<Hall*> danhSachSanh;
    vector<BookingRequest*> danhSachBooking;
    int soThuTuSuKien;  // dùng sinh mã sự kiện tự tăng (SK001, SK002, ...)
    int soThuTuBooking; // dùng sinh mã booking tự tăng (BK001, BK002, ...)

    BookingRequest* timBooking(const string& maBooking) const; // ném InvalidStateTransitionException nếu không tồn tại
    string taoMaSuKien();
    string taoMaBooking();

public:
    HeThongDatLich();
    ~HeThongDatLich();
    void khoiTaoDuLieuMau(); // dữ liệu mẫu ban đầu cho dễ demo

    const vector<Hall*>& getDanhSachSanh() const;
    const vector<BookingRequest*>& getDanhSachBooking() const;

    // ----- FR-01: quản lý danh mục sảnh -----
    void themSanh(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong);
    void suaSanh(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong);
    void xoaSanh(const string& maSanh);
    Hall* timSanh(const string& maSanh) const; // ném HallNotFoundException
    void xemDanhSachSanh() const;

    // ----- FR-04 (tạo yêu cầu) + FR-05 (kiểm tra trùng lịch) -----
    BookingRequest* taoYeuCauDatLich(const string& maSanh, int loaiSuKien,
                                     const string& tenSuKien, int soKhach,
                                     const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc);
    void kiemTraTrungLich(const Hall* sanh, const ThoiDiem& bd, const ThoiDiem& kt,
                          const string& maBookingBoQua) const;

    // ----- FR-06: duyệt | FR-07: từ chối | FR-08: hủy | FR-09: đổi lịch -----
    void duyetYeuCau(const string& maBooking);
    void tuChoiYeuCau(const string& maBooking, const string& lyDo);
    void huyLichDaDuyet(const string& maBooking);
    void doiLich(const string& maBooking, const ThoiDiem& gioBatDauMoi, const ThoiDiem& gioKetThucMoi);

    // ----- FR-03: xem lịch trống của sảnh theo ngày -----
    void xemLichTrong(const string& maSanh, int ngay, int thang, int nam) const;

    // ----- FR-10: tra cứu danh sách lịch -----
    vector<BookingRequest*> traCuu(bool locTrangThai, TrangThai trangThaiLoc,
                                   bool locSanh, const string& maSanhLoc,
                                   bool locNgay, const ThoiDiem& tuNgay, const ThoiDiem& denNgay) const;
};

#endif // HETHONGDATLICH_H
