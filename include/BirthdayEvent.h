#ifndef BIRTHDAYEVENT_H
#define BIRTHDAYEVENT_H

#include "Event.h"

using namespace std;

// =====================================================================
// Lớp con TIỆC SINH NHẬT — kế thừa Event (kế thừa + đa hình, FR-02):
//   - Chuẩn bị trước: 1 giờ, dọn dẹp sau: 0.5 giờ
// =====================================================================
class BirthdayEvent : public Event {
public:
    BirthdayEvent(const string& maSuKien, const string& tenSuKien,
                  int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc);

    double thoiGianChuanBi() const override;    // 1 giờ
    double thoiGianDonDep() const override;     // 0.5 giờ
    string tenLoai() const override;            // "Sinh nhat"
    int maLoai() const override;                // 3
};

#endif // BIRTHDAYEVENT_H
