#ifndef CONFERENCEEVENT_H
#define CONFERENCEEVENT_H

#include "Event.h"

using namespace std;

// =====================================================================
// Lớp con HỘI NGHỊ — kế thừa Event (kế thừa + đa hình, FR-02):
//   - Chuẩn bị trước: 1 giờ, dọn dẹp sau: 1 giờ
//   - Điều kiện riêng: phải đặt trước tối thiểu 3 ngày
// =====================================================================
class ConferenceEvent : public Event {
public:
    ConferenceEvent(const string& maSuKien, const string& tenSuKien,
                    int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc);

    double thoiGianChuanBi() const override;    // 1 giờ
    double thoiGianDonDep() const override;     // 1 giờ
    void kiemTraDieuKienRieng() const override; // đặt trước >= 3 ngày
    string tenLoai() const override;            // "Hoi nghi"
};

#endif // CONFERENCEEVENT_H
