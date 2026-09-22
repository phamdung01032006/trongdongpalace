#ifndef WEDDINGEVENT_H
#define WEDDINGEVENT_H

#include "Event.h"

using namespace std;

// =====================================================================
// Lớp con TIỆC CƯỚI — kế thừa Event (kế thừa + đa hình, FR-02):
//   - Chuẩn bị trước: 3 giờ, dọn dẹp sau: 2 giờ
//   - Điều kiện riêng: số khách tối thiểu 50
// =====================================================================
class WeddingEvent : public Event {
public:
    WeddingEvent(const string& maSuKien, const string& tenSuKien,
                 int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc);

    double thoiGianChuanBi() const override;    // 3 giờ
    double thoiGianDonDep() const override;     // 2 giờ
    void kiemTraDieuKienRieng() const override; // soKhach >= 50
    string tenLoai() const override;            // "Tiec cuoi"
    int maLoai() const override;                // 1
};

#endif // WEDDINGEVENT_H
