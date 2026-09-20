#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "Utils.h"

using namespace std;

// =====================================================================
// Lớp TRỪU TƯỢNG mô tả một sự kiện sử dụng sảnh (Mục 6, FR-02).
//
// KẾ THỪA + ĐA HÌNH: Event là lớp cha, có 3 hàm thuần ảo (pure virtual).
// Các lớp con WeddingEvent / ConferenceEvent / BirthdayEvent override
// 3 hàm này theo đặc thù của từng loại sự kiện. Trong FR-04, hệ thống
// chỉ cần giữ con trỏ Event* và gọi kiemTraDieuKienRieng(),
// thoiGianChuanBi(), thoiGianDonDep() — phiên bản của lớp con tương
// ứng sẽ được thực thi đúng lúc chạy (dynamic dispatch).
// =====================================================================
class Event {
protected:
    string maSuKien;
    string tenSuKien;
    int soKhach;
    ThoiDiem gioBatDau;  // giờ bắt đầu chính thức (do người yêu cầu nhập)
    ThoiDiem gioKetThuc; // giờ kết thúc chính thức

public:
    Event(const string& maSuKien, const string& tenSuKien,
          int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc);
    virtual ~Event();

    // ----- Các hàm thuần ảo, bắt buộc lớp con override -----
    virtual double thoiGianChuanBi() const = 0;    // số giờ chuẩn bị TRƯỚC sự kiện
    virtual double thoiGianDonDep() const = 0;     // số giờ dọn dẹp SAU sự kiện
    virtual void kiemTraDieuKienRieng() const = 0; // ném EventConditionViolationException nếu vi phạm
    virtual string tenLoai() const = 0;            // tên loại sự kiện để hiển thị

    // ----- Hàm dùng chung (kế thừa được từ mọi lớp con) -----
    // Khung giờ "mở rộng" sau khi cộng thêm thời gian chuẩn bị/dọn dẹp.
    // Đây chính là khung giờ dùng để kiểm tra trùng lịch (FR-05).
    ThoiDiem batDauRong() const;
    ThoiDiem ketThucRong() const;

    string getMaSuKien() const;
    string getTenSuKien() const;
    int getSoKhach() const;
    ThoiDiem getGioBatDau() const;
    ThoiDiem getGioKetThuc() const;
    void setKhungGio(const ThoiDiem& gioBatDauMoi, const ThoiDiem& gioKetThucMoi); // dùng cho FR-09
};

#endif // EVENT_H
