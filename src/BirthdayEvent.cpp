#include "BirthdayEvent.h"
#include "Exceptions.h"

using namespace std;

BirthdayEvent::BirthdayEvent(const string& maSuKien, const string& tenSuKien,
                             int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc)
    : Event(maSuKien, tenSuKien, soKhach, gioBatDau, gioKetThuc) {}

double BirthdayEvent::thoiGianChuanBi() const { return 1.0; }  // 1 giờ
double BirthdayEvent::thoiGianDonDep() const { return 0.5; }   // 0.5 giờ

// Điều kiện riêng: tiệc sinh nhật tối đa 100 khách
void BirthdayEvent::kiemTraDieuKienRieng() const {
    if (soKhach > 100) {
        throw EventConditionViolationException(
            "Tiec sinh nhat chi toi da 100 khach (hien tai " + to_string(soKhach) + " khach)");
    }
}

string BirthdayEvent::tenLoai() const { return "Sinh nhat"; }
int BirthdayEvent::maLoai() const { return 3; } // mã loại dùng khi lưu/đọc file
