#include "ConferenceEvent.h"
#include "Exceptions.h"

using namespace std;

ConferenceEvent::ConferenceEvent(const string& maSuKien, const string& tenSuKien,
                                 int soKhach, const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc)
    : Event(maSuKien, tenSuKien, soKhach, gioBatDau, gioKetThuc) {}

double ConferenceEvent::thoiGianChuanBi() const { return 1.0; } // 1 giờ
double ConferenceEvent::thoiGianDonDep() const { return 1.0; }  // 1 giờ

// Điều kiện riêng: hội nghị phải đặt trước tối thiểu 3 ngày
void ConferenceEvent::kiemTraDieuKienRieng() const {
    ThoiDiem hanDatTruoc = congPhut(hienTai(), 3LL * 24 * 60); // mốc hôm nay + 3 ngày
    if (gioBatDau < hanDatTruoc) {
        throw EventConditionViolationException(
            "Hoi nghi phai dat truoc toi thieu 3 ngay (dat truoc muon nhat vao "
            + hanDatTruoc.toString() + ")");
    }
}

string ConferenceEvent::tenLoai() const { return "Hoi nghi"; }
