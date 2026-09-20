#include "WeddingEvent.h"
#include "Exceptions.h"

using namespace std;

WeddingEvent::WeddingEvent(const string& maSuKien, const string& tenSuKien,
                           int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc)
    : Event(maSuKien, tenSuKien, soKhach, gioBatDau, gioKetThuc) {}

// ĐA HÌNH: mỗi lớp con trả về thời gian chuẩn bị/dọn dẹp riêng của mình
double WeddingEvent::thoiGianChuanBi() const { return 3.0; } // 3 giờ
double WeddingEvent::thoiGianDonDep() const { return 2.0; }  // 2 giờ

// Điều kiện riêng: tiệc cưới cần tối thiểu 50 khách
void WeddingEvent::kiemTraDieuKienRieng() const {
    if (soKhach < 50) {
        throw EventConditionViolationException(
            "Tiec cuoi doi toi thieu 50 khach (hien tai chi co " + to_string(soKhach) + " khach)");
    }
}

string WeddingEvent::tenLoai() const { return "Tiec cuoi"; }
