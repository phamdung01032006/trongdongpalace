#include "BirthdayEvent.h"
#include "Exceptions.h"

using namespace std;

BirthdayEvent::BirthdayEvent(const string& maSuKien, const string& tenSuKien,
                             int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc)
    : Event(maSuKien, tenSuKien, soKhach, gioBatDau, gioKetThuc) {}

double BirthdayEvent::thoiGianChuanBi() const { return 1.0; }  // 1 giờ
double BirthdayEvent::thoiGianDonDep() const { return 0.5; }   // 0.5 giờ

string BirthdayEvent::tenLoai() const { return "Sinh nhat"; }
int BirthdayEvent::maLoai() const { return 3; } // mã loại dùng khi lưu/đọc file
