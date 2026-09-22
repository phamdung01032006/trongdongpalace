#include "HeThongDatLich.h"
#include "WeddingEvent.h"
#include "ConferenceEvent.h"
#include "BirthdayEvent.h"
#include "Utils.h"
#include <algorithm>
#include <iomanip>

using namespace std;

// ================== Khởi tạo / giải phóng ==================
HeThongDatLich::HeThongDatLich() : soThuTuSuKien(0), soThuTuBooking(0) {}

HeThongDatLich::~HeThongDatLich() {
    // Lưu toàn bộ dữ liệu ra file txt trước khi giải phóng bộ nhớ, đảm bảo
    // dữ liệu không bị mất khi chương trình được chạy lại.
    ghiDuLieuRaFile();
    // Giải phóng toàn bộ bộ nhớ: booking (tự delete Event bên trong) và sảnh
    for (BookingRequest* b : danhSachBooking) delete b;
    for (Hall* h : danhSachSanh) delete h;
}

// Nạp dữ liệu đã lưu từ file txt (gọi ngay sau khi tạo đối tượng HeThongDatLich)
void HeThongDatLich::docDuLieuTuFile() {
    // Gọi hàm toàn cục trong Utils (tên khác method nên không bị che khuất)
    napHeThongTuFile(danhSachSanh, danhSachBooking);
    capNhatSoThuTu(); // tránh sinh mã SK/BK trùng với dữ liệu vừa nạp
}

// Ghi toàn bộ dữ liệu ra file txt
void HeThongDatLich::ghiDuLieuRaFile() {
    luuHeThongRaFile(danhSachSanh, danhSachBooking);
}

// Khôi phục 2 bộ đếm mã sau khi nạp dữ liệu từ file.
// Quét mã sự kiện (SK###) và mã booking (BK###), lấy số lớn nhất làm bộ đếm
// để các mã sinh ra tiếp theo không bị trùng với dữ liệu đã lưu.
void HeThongDatLich::capNhatSoThuTu() {
    soThuTuSuKien = 0;
    soThuTuBooking = 0;

    for (BookingRequest* b : danhSachBooking) {
        // Mã booking có dạng BK###
        const string& maBK = b->getMaBooking();
        if (maBK.size() > 2 && maBK[0] == 'B' && maBK[1] == 'K') {
            int so = 0;
            try { so = stoi(maBK.substr(2)); } catch (const exception&) { so = 0; }
            if (so > soThuTuBooking) soThuTuBooking = so;
        }
        // Mã sự kiện có dạng SK###
        Event* sk = b->getSuKien();
        if (sk == nullptr) continue;
        const string& maSK = sk->getMaSuKien();
        if (maSK.size() > 2 && maSK[0] == 'S' && maSK[1] == 'K') {
            int so = 0;
            try { so = stoi(maSK.substr(2)); } catch (const exception&) { so = 0; }
            if (so > soThuTuSuKien) soThuTuSuKien = so;
        }
    }
}

// Dữ liệu mẫu để demo nhanh (FR-01, FR-02)
void HeThongDatLich::khoiTaoDuLieuMau() {
    if (!danhSachSanh.empty()) return;
    themSanh("S01", "Sanh Kim Tuyet", 500, true);
    themSanh("S02", "Sanh Hoa Hong", 200, true);
    themSanh("S03", "Sanh Phong Ca", 80, true);
    themSanh("S04", "Sanh Nguyen Phong", 300, false); // sảnh ngừng hoạt động
}

// ================== Sinh mã tự động ==================
string HeThongDatLich::taoMaSuKien() {
    ++soThuTuSuKien;
    ostringstream os;
    os << "SK" << setfill('0') << setw(3) << soThuTuSuKien;
    return os.str();
}

string HeThongDatLich::taoMaBooking() {
    ++soThuTuBooking;
    ostringstream os;
    os << "BK" << setfill('0') << setw(3) << soThuTuBooking;
    return os.str();
}

// ================== Tra cứu nội bộ ==================
Hall* HeThongDatLich::timSanh(const string& maSanh) const {
    for (Hall* h : danhSachSanh) {
        if (h->getMaSanh() == maSanh) return h;
    }
    throw HallNotFoundException("Khong tim thay sanh co ma \"" + maSanh + "\"");
}

BookingRequest* HeThongDatLich::timBooking(const string& maBooking) const {
    for (BookingRequest* b : danhSachBooking) {
        if (b->getMaBooking() == maBooking) return b;
    }
    throw InvalidStateTransitionException(
        "Booking \"" + maBooking + "\" khong ton tai trong he thong");
}

const vector<Hall*>& HeThongDatLich::getDanhSachSanh() const { return danhSachSanh; }
const vector<BookingRequest*>& HeThongDatLich::getDanhSachBooking() const { return danhSachBooking; }

// ================== FR-01: Quản lý danh mục sảnh ==================
void HeThongDatLich::themSanh(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong) {
    if (sucChua <= 0) {
        throw InvalidCapacityException("Suc chua phai la so duong (ban nhap: " + to_string(sucChua) + ")");
    }
    // Tránh trùng mã sảnh
    for (Hall* h : danhSachSanh) {
        if (h->getMaSanh() == maSanh) {
            throw HallNotFoundException("Ma sanh \"" + maSanh + "\" da ton tai, vui long dung ma khac");
        }
    }
    danhSachSanh.push_back(new Hall(maSanh, ten, sucChua, dangHoatDong));
    ghiDuLieuRaFile(); // tự động lưu ngay để không mất dữ liệu nếu tắt đột ngột
}

void HeThongDatLich::suaSanh(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong) {
    Hall* h = timSanh(maSanh); // ném HallNotFoundException nếu không thấy
    if (sucChua <= 0) {
        throw InvalidCapacityException("Suc chua phai la so duong (ban nhap: " + to_string(sucChua) + ")");
    }
    h->setTen(ten);
    h->setSucChua(sucChua);
    h->setDangHoatDong(dangHoatDong);
    ghiDuLieuRaFile(); // tự động lưu sau khi sửa sảnh
}

void HeThongDatLich::xoaSanh(const string& maSanh) {
    Hall* h = timSanh(maSanh);
    // Không cho xóa sảnh đang có booking Approved diễn ra trong tương lai
    ThoiDiem bayGio = hienTai();
    for (BookingRequest* b : danhSachBooking) {
        if (b->getSanh() == h && b->getTrangThai() == APPROVED
            && b->getSuKien()->getGioKetThuc() >= bayGio) {
            throw HallInUseException(
                "Sanh " + maSanh + " dang co booking [" + b->getMaBooking()
                + "] da duyet con hieu luc, khong the xoa");
        }
    }
    // Xóa luôn các booking ràng buộc tới sảnh này (Pending/Rejected/Cancelled cũ)
    for (auto it = danhSachBooking.begin(); it != danhSachBooking.end();) {
        if ((*it)->getSanh() == h) {
            delete* it;
            it = danhSachBooking.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = danhSachSanh.begin(); it != danhSachSanh.end(); ++it) {
        if (*it == h) {
            danhSachSanh.erase(it);
            break;
        }
    }
    delete h;
    ghiDuLieuRaFile(); // tự động lưu sau khi xóa sảnh
}

void HeThongDatLich::xemDanhSachSanh() const {
    if (danhSachSanh.empty()) {
        cout << "  (Danh muc sanh trong)\n";
        return;
    }

    // Tiêu đề bảng và độ rộng từng cột được tính TỰ ĐỘNG theo ô dài nhất
    // (so sánh giữa tiêu đề và toàn bộ dữ liệu) nên bảng luôn thẳng hàng.
    const vector<string> tieuDe = {"Ma", "Ten sanh", "Suc chua", "Trang thai"};
    const size_t soCot = tieuDe.size();

    vector<int> doRong(soCot);
    for (size_t c = 0; c < soCot; ++c) doRong[c] = doRongHienThi(tieuDe[c]);
    for (Hall* h : danhSachSanh) {
        vector<string> o = {h->getMaSanh(), h->getTen(),
                            to_string(h->getSucChua()) + " khach",
                            h->getDangHoatDong() ? "Dang hoat dong" : "Ngung hoat dong"};
        for (size_t c = 0; c < soCot; ++c) {
            doRong[c] = max(doRong[c], doRongHienThi(o[c]));
        }
    }

    string ke = duongKeBang(doRong);
    cout << "  " << ke << "\n";
    cout << "  ";
    inDongBang(tieuDe, doRong, {false, false, true, false});
    cout << "  " << ke << "\n";
    for (Hall* h : danhSachSanh) {
        cout << "  ";
        h->hienThi(doRong);
    }
    cout << "  " << ke << "\n";
}

// ================== FR-05: Kiểm tra trùng lịch ==================
// So khung giờ mới (đã cộng chuẩn bị/dọn dẹp) với mọi booking
// Pending/Approved trên cùng sảnh. `maBookingBoQua` dùng khi đổi lịch (FR-09).
void HeThongDatLich::kiemTraTrungLich(const Hall* sanh, const ThoiDiem& bd, const ThoiDiem& kt,
                                      const string& maBookingBoQua) const {
    for (BookingRequest* b : danhSachBooking) {
        if (b->getSanh() != sanh) continue;                       // khác sảnh -> bỏ qua
        if (b->getMaBooking() == maBookingBoQua) continue;        // chính nó -> bỏ qua
        TrangThai tt = b->getTrangThai();
        if (tt != PENDING && tt != APPROVED) continue;            // chỉ xét Pending/Approved

        Event* sk = b->getSuKien();
        if (haiKhungGiaoNhau(bd, kt, sk->batDauRong(), sk->ketThucRong())) {
            throw DoubleBookingException(
                "Trung lich voi booking [" + b->getMaBooking() + "] ("
                + sk->tenLoai() + " " + sk->getGioBatDau().toString() + " -> "
                + sk->getGioKetThuc().toString() + ", khung rong "
                + sk->batDauRong().toString() + " -> " + sk->ketThucRong().toString() + ")");
        }
    }
}

// ================== FR-04: Tạo yêu cầu đặt lịch ==================
BookingRequest* HeThongDatLich::taoYeuCauDatLich(const string& maSanh, int loaiSuKien,
                                                 const string& tenSuKien, int soKhach,
                                                 const ThoiDiem& gioBatDau, const ThoiDiem& gioKetThuc) {
    // 1. Sảnh phải tồn tại và đang hoạt động
    Hall* sanh = timSanh(maSanh); // ném HallNotFoundException
    if (!sanh->getDangHoatDong()) {
        throw InvalidStateTransitionException("Sanh \"" + maSanh + "\" dang ngung hoat dong, khong the dat");
    }
    if (gioKetThuc < gioBatDau) {
        throw InvalidDateException("Gio ket thuc phai sau gio bat dau");
    }

    // 2. Sức chứa sảnh phải đủ cho số khách (FR-04 luồng ngoại lệ 1)
    if (soKhach > sanh->getSucChua()) {
        throw InvalidCapacityException(
            "So khach (" + to_string(soKhach) + ") vuot suc chua sanh \""
            + sanh->getTen() + "\" (toi da " + to_string(sanh->getSucChua()) + " khach)");
    }

    // 3. ĐA HÌNH: khởi tạo đúng đối tượng Event theo loại được chọn.
    //    Ở đây chỉ cần biết lớp cha Event — hành vi cụ thể do lớp con quyết định.
    Event* suKien = nullptr;
    switch (loaiSuKien) {
        case 1: suKien = new WeddingEvent(taoMaSuKien(), tenSuKien, soKhach, gioBatDau, gioKetThuc); break;
        case 2: suKien = new ConferenceEvent(taoMaSuKien(), tenSuKien, soKhach, gioBatDau, gioKetThuc); break;
        case 3: suKien = new BirthdayEvent(taoMaSuKien(), tenSuKien, soKhach, gioBatDau, gioKetThuc); break;
        default:
            throw EventConditionViolationException("Loai su kien khong hop le (chi nhan 1/2/3)");
    }

    try {
        // 4. ĐA HÌNH (FR-04 bước 3): gọi hàm ảo qua con trỏ Event*
        //    -> phiên bản kiemTraDieuKienRieng() của lớp con được thực thi
        suKien->kiemTraDieuKienRieng();

        // 5. FR-05: kiểm tra trùng lịch trên khung giờ MỞ RỘNG (đã cộng
        //    chuẩn bị/dọn dẹp qua 2 hàm ảo thoiGianChuanBi()/thoiGianDonDep())
        kiemTraTrungLich(sanh, suKien->batDauRong(), suKien->ketThucRong(), "");
    } catch (...) {
        delete suKien; // tránh rò rỉ bộ nhớ khi kiểm tra thất bại
        throw;
    }

    // 6. Hợp lệ -> tạo booking ở trạng thái Pending (post-condition FR-04)
    BookingRequest* booking = new BookingRequest(taoMaBooking(), sanh, suKien);
    danhSachBooking.push_back(booking);
    ghiDuLieuRaFile(); // tự động lưu booking mới
    return booking;
}

// ================== FR-06: Duyệt yêu cầu ==================
void HeThongDatLich::duyetYeuCau(const string& maBooking) {
    BookingRequest* b = timBooking(maBooking);
    if (b->getTrangThai() != PENDING) {
        throw InvalidStateTransitionException(
            "Chi duyet duoc booking o trang thai Pending (booking " + maBooking
            + " dang o trang thai: " + trangThaiThanhChu(b->getTrangThai()) + ")");
    }
    // Kiểm tra LẠI tính khả dụng đề phòng booking khác vừa được duyệt (FR-06)
    Event* sk = b->getSuKien();
    kiemTraTrungLich(b->getSanh(), sk->batDauRong(), sk->ketThucRong(), b->getMaBooking());
    b->setTrangThai(APPROVED);
    ghiDuLieuRaFile(); // tự động lưu sau khi duyệt
}

// ================== FR-07: Từ chối yêu cầu ==================
void HeThongDatLich::tuChoiYeuCau(const string& maBooking, const string& lyDo) {
    BookingRequest* b = timBooking(maBooking);
    if (b->getTrangThai() != PENDING) {
        throw InvalidStateTransitionException(
            "Chi tu choi duoc booking o trang thai Pending (booking " + maBooking
            + " dang o trang thai: " + trangThaiThanhChu(b->getTrangThai()) + ")");
    }
    b->setLyDoTuChoi(lyDo);
    b->setTrangThai(REJECTED);
    ghiDuLieuRaFile(); // tự động lưu sau khi từ chối
}

// ================== FR-08: Hủy lịch đã duyệt ==================
void HeThongDatLich::huyLichDaDuyet(const string& maBooking) {
    BookingRequest* b = timBooking(maBooking);
    if (b->getTrangThai() != APPROVED) {
        throw InvalidStateTransitionException(
            "Chi huy duoc booking o trang thai Approved (booking " + maBooking
            + " dang o trang thai: " + trangThaiThanhChu(b->getTrangThai()) + ")");
    }
    // Hạn hủy: phải hủy trước 48 giờ so với giờ tổ chức (FR-08)
    ThoiDiem bayGio = hienTai();
    ThoiDiem hanHuy = congPhut(b->getSuKien()->getGioBatDau(), -(48LL * 60));
    if (bayGio >= hanHuy) {
        throw LateCancellationException(
            "Huy tre: phai huy truoc 48 gio so voi gio to chuc ("
            + b->getSuKien()->getGioBatDau().toString()
            + "), han chuan la " + hanHuy.toString()
            + " con bay gio la " + bayGio.toString());
    }
    b->setTrangThai(CANCELLED); // post-condition: khung giờ được giải phóng
    ghiDuLieuRaFile();          // tự động lưu sau khi hủy lịch
}

// ================== FR-09: Đổi lịch (reschedule) ==================
void HeThongDatLich::doiLich(const string& maBooking, const ThoiDiem& gioBatDauMoi, const ThoiDiem& gioKetThucMoi) {
    BookingRequest* b = timBooking(maBooking);
    TrangThai tt = b->getTrangThai();
    if (tt != PENDING && tt != APPROVED) {
        throw InvalidStateTransitionException(
            "Chi doi lich duoc booking Pending hoac Approved (booking " + maBooking
            + " dang o trang thai: " + trangThaiThanhChu(tt) + ")");
    }
    if (gioKetThucMoi < gioBatDauMoi) {
        throw InvalidDateException("Gio ket thuc moi phai sau gio bat dau moi");
    }

    Event* sk = b->getSuKien();
    ThoiDiem bdCu = sk->getGioBatDau();
    ThoiDiem ktCu = sk->getGioKetThuc();

    // Tạm đặt khung giờ mới để tính lại khung mở rộng theo đúng loại sự kiện
    sk->setKhungGio(gioBatDauMoi, gioKetThucMoi);
    try {
        kiemTraTrungLich(b->getSanh(), sk->batDauRong(), sk->ketThucRong(), b->getMaBooking());
    } catch (const DoubleBookingException&) {
        // Khung giờ mới trùng lịch -> trả về khung cũ, booking KHÔNG bị mất (FR-09)
        sk->setKhungGio(bdCu, ktCu);
        throw;
    }
    // Đổi lịch thành công, giữ nguyên mã booking và lịch sử
    ghiDuLieuRaFile(); // chỉ lưu khi đổi lịch THÀNH CÔNG (nhánh lỗi ở trên đã throw)
}



// ================== FR-03: Xem lịch trống của sảnh theo ngày ==================
void HeThongDatLich::xemLichTrong(const string& maSanh, int ngay, int thang, int nam) const {
    Hall* sanh = timSanh(maSanh); // ném HallNotFoundException
    ThoiDiem dauNgay(ngay, thang, nam, 0, 0);
    ThoiDiem cuoiNgay = congPhut(dauNgay, 24LL * 60);

    cout << "\n  Lich cua sanh " << sanh->getMaSanh() << " (" << sanh->getTen()
         << ") ngay " << dauNgay.toString() << ":\n";

    // Gom các khung giờ đã bị chiếm (booking Approved) mà giao với ngày đang xem
    vector<pair<ThoiDiem, ThoiDiem>> cacKhungBiChiem;
    for (BookingRequest* b : danhSachBooking) {
        if (b->getSanh() != sanh || b->getTrangThai() != APPROVED) continue;
        Event* sk = b->getSuKien();
        ThoiDiem bd = sk->batDauRong(); // khung giờ mở rộng mới bị coi là bận
        ThoiDiem kt = sk->ketThucRong();
        if (bd < cuoiNgay && dauNgay < kt) {
            if (bd < dauNgay) bd = dauNgay;   // cắt mép về trong ngày để hiển thị
            if (cuoiNgay < kt) kt = cuoiNgay;
            cacKhungBiChiem.push_back(make_pair(bd, kt));
        }
    }

    if (cacKhungBiChiem.empty()) {
        cout << "  - Khong co su kien nao: toan bo ngay CON TRONG (00:00 -> 24:00).\n";
        return;
    }

    // Sắp xếp theo giờ bắt đầu rồi "dùng thước" đi qua từng khung bận
    sort(cacKhungBiChiem.begin(), cacKhungBiChiem.end(),
         [](const pair<ThoiDiem, ThoiDiem>& a, const pair<ThoiDiem, ThoiDiem>& b) {
             return a.first < b.first;
         });

    cout << "  - Cac khung da co nguoi dat:\n";
    for (const auto& k : cacKhungBiChiem) {
        cout << "      * " << k.first.toString() << " -> " << k.second.toString() << "\n";
    }

    cout << "  - Cac khung con trong trong ngay:\n";
    ThoiDiem conTro = dauNgay;
    bool coTrong = false;
    for (const auto& k : cacKhungBiChiem) {
        if (conTro < k.first) {
            cout << "      * " << conTro.toString() << " -> " << k.first.toString() << "\n";
            coTrong = true;
        }
        if (conTro < k.second) conTro = k.second;
    }
    if (conTro < cuoiNgay) {
        cout << "      * " << conTro.toString() << " -> " << cuoiNgay.toString() << "\n";
        coTrong = true;
    }
    if (!coTrong) {
        cout << "      * (Khong con khoang thoi gian nao trong trong ngay)\n";
    }
}

// ================== FR-10: Tra cứu danh sách lịch ==================
vector<BookingRequest*> HeThongDatLich::traCuu(bool locTrangThai, TrangThai trangThaiLoc,
                                               bool locSanh, const string& maSanhLoc,
                                               bool locNgay, const ThoiDiem& tuNgay, const ThoiDiem& denNgay) const {
    vector<BookingRequest*> ketQua;
    for (BookingRequest* b : danhSachBooking) {
        if (locTrangThai && b->getTrangThai() != trangThaiLoc) continue;
        if (locSanh && b->getSanh()->getMaSanh() != maSanhLoc) continue;
        if (locNgay) {
            ThoiDiem bd = b->getSuKien()->getGioBatDau();
            if (bd < tuNgay || denNgay < bd) continue;
        }
        ketQua.push_back(b);
    }
    return ketQua;
}
