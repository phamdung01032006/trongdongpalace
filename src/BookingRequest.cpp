#include "BookingRequest.h"

using namespace std;

string trangThaiThanhChu(TrangThai t) {
    switch (t) {
        case PENDING:  return "Pending (cho duyet)";
        case APPROVED: return "Approved (da duyet)";
        case REJECTED: return "Rejected (tu choi)";
        case CANCELLED: return "Cancelled (da huy)";
    }
    return "Khong xac dinh";
}

BookingRequest::BookingRequest(const string& maBooking, Hall* sanh, Event* suKien)
    : maBooking(maBooking), sanh(sanh), suKien(suKien),
      trangThai(PENDING), ngayTao(hienTai()), lyDoTuChoi("") {}

BookingRequest::~BookingRequest() {
    // BookingRequest sở hữu Event* nên có trách nhiệm giải phóng.
    // Hall* do danh mục sảnh sở hữu, không giải phóng tại đây.
    delete suKien;
}

string BookingRequest::getMaBooking() const { return maBooking; }
TrangThai BookingRequest::getTrangThai() const { return trangThai; }
Hall* BookingRequest::getSanh() const { return sanh; }
Event* BookingRequest::getSuKien() const { return suKien; }
ThoiDiem BookingRequest::getNgayTao() const { return ngayTao; }
string BookingRequest::getLyDoTuChoi() const { return lyDoTuChoi; }

void BookingRequest::setTrangThai(TrangThai trangThaiMoi) { trangThai = trangThaiMoi; }
void BookingRequest::setLyDoTuChoi(const string& lyDo) { lyDoTuChoi = lyDo; }

void BookingRequest::hienThi() const {
    cout << "  [" << maBooking << "] " << suKien->tenLoai() << " \"" << suKien->getTenSuKien() << "\""
         << " | Sanh " << sanh->getMaSanh() << " (" << sanh->getTen() << ")"
         << " | " << suKien->getSoKhach() << " khach"
         << " | Gio to chuc: " << suKien->getGioBatDau().toString()
         << " -> " << suKien->getGioKetThuc().toString()
         << " | Khung rong: " << suKien->batDauRong().toString()
         << " -> " << suKien->ketThucRong().toString()
         << " | " << trangThaiThanhChu(trangThai);
    if (!lyDoTuChoi.empty()) cout << " | Ly do: " << lyDoTuChoi;
    cout << "\n";
}
