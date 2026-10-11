#include "Event.h"
#include "Exceptions.h"

using namespace std;

Event::Event(const string& maSuKien, const string& tenSuKien,
             int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc)
    : maSuKien(maSuKien), tenSuKien(tenSuKien), soKhach(soKhach),
      gioBatDau(gioBatDau), gioKetThuc(gioKetThuc) {}

Event::~Event() {}

// ----- Khung giờ "mở rộng" (cộng chuẩn bị trước / dọn dẹp sau) -----
// Ví dụ: tiệc cưới 17:00-20:00 => khung rộng 14:00-22:00
// (chuẩn bị 3h trước + dọn dẹp 2h sau).
ThoiDiem Event::batDauRong() const {
    long long soPhutChuanBi = (long long)(thoiGianChuanBi() * 60.0 + 0.5); // làm tròn phút
    return congPhut(gioBatDau, -soPhutChuanBi);
}

ThoiDiem Event::ketThucRong() const {
    long long soPhutDonDep = (long long)(thoiGianDonDep() * 60.0 + 0.5);
    return congPhut(gioKetThuc, soPhutDonDep);
}

string Event::getMaSuKien() const { return maSuKien; }
string Event::getTenSuKien() const { return tenSuKien; }
int Event::getSoKhach() const { return soKhach; }
ThoiDiem Event::getGioBatDau() const { return gioBatDau; }
ThoiDiem Event::getGioKetThuc() const { return gioKetThuc; }

// Dùng cho FR-09 (đổi lịch): chỉ thay khung giờ, giữ nguyên mọi thông tin khác
void Event::setKhungGio(const ThoiDiem& gioBatDauMoi, const ThoiDiem& gioKetThucMoi) {
    gioBatDau = gioBatDauMoi;
    gioKetThuc = gioKetThucMoi;
}
