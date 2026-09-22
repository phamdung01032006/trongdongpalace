// =====================================================================
// main.cpp — Giao diện console (CLI) của Hệ thống Đặt lịch Sự kiện.
//
// Chương trình chạy theo vai trò (actor) được chọn từ menu:
//   1) NGƯỜI YÊU CẦU  : xem lịch trống, tạo/hủy/đổi booking, tra cứu
//   2) QUẢN LÝ SẢNH   : quản lý danh mục sảnh, duyệt/từ chối booking
//
// Toàn bộ nghiệp vụ nằm trong lớp HeThongDatLich; file này chỉ vẽ
// menu, thu thập dữ liệu nhập và BẮT NGOẠI LỆ (FR-11) để hiển thị.
// =====================================================================

#include <iostream>
#include <iomanip>
#include "HeThongDatLich.h"
#include "Exceptions.h"
#include "Utils.h"

using namespace std;

// In thông báo lỗi từ exception ra màn hình (FR-11: không để chương trình chết)
void inLoi(const exception& e) {
    cout << "\n  [LOI] " << e.what() << "\n";
}

// Hiển thị danh sách booking theo bộ lọc (dùng chung cho FR-10)
void traCuuLichUI(HeThongDatLich& ht) {
    cout << "  Tra cuu theo: 1. Tat ca  2. Theo trang thai  3. Theo sanh  4. Theo khoang ngay\n";
    int chon = docSoNguyen("  Lua chon: ");
    bool locT = false, locS = false, locN = false;
    TrangThai tt = PENDING;
    string maS;
    ThoiDiem tuNgay, denNgay;

    switch (chon) {
        case 1: break;
        case 2: {
            int i = docSoNguyen("  Trang thai (0=Pending 1=Approved 2=Rejected 3=Cancelled): ");
            if (i < 0 || i > 3) { cout << "  Trang thai khong hop le.\n"; return; }
            tt = (TrangThai)i;
            locT = true;
            break;
        }
        case 3: maS = docChuoi("  Ma sanh: "); locS = true; break;
        case 4: {
            int d, m, y;
            docNgay("  Tu ngay", d, m, y);
            tuNgay = ThoiDiem(d, m, y, 0, 0);
            docNgay("  Den ngay", d, m, y);
            denNgay = congPhut(ThoiDiem(d, m, y, 0, 0), 24LL * 60 - 1); // tính cả ngày đến
            locN = true;
            break;
        }
        default: cout << "  Lua chon khong hop le.\n"; return;
    }

    vector<BookingRequest*> ketQua = ht.traCuu(locT, tt, locS, maS, locN, tuNgay, denNgay);
    if (ketQua.empty()) {
        cout << "  Khong co booking nao phu hop (danh sach rong).\n"; // FR-10: rỗng không phải lỗi
        return;
    }
    cout << "  Tim thay " << ketQua.size() << " booking:\n";
    for (BookingRequest* b : ketQua) b->hienThi();
}

// Liệt kê các booking đang ở trạng thái chỉ định (phục vụ duyệt/hủy/đổi lịch)
void lietKeTheoTrangThai(HeThongDatLich& ht, TrangThai tt) {
    cout << "  Danh sach booking trang thai: " << trangThaiThanhChu(tt) << "\n";
    bool co = false;
    for (BookingRequest* b : ht.getDanhSachBooking()) {
        if (b->getTrangThai() == tt) {
            b->hienThi();
            co = true;
        }
    }
    if (!co) cout << "  (Khong co booking nao)\n";
}

// Xem lịch trống của một sảnh theo ngày (FR-03)
void xemLichTrongUI(HeThongDatLich& ht) {
    ht.xemDanhSachSanh();
    string maSanh = docChuoi("  Ma sanh: ");
    int d, m, y;
    docNgay("  Ngay can xem", d, m, y);
    ht.xemLichTrong(maSanh, d, m, y);
}

// Tạo yêu cầu đặt lịch (FR-04)
void taoBookingUI(HeThongDatLich& ht) {
    ht.xemDanhSachSanh();
    string maSanh = docChuoi("  Ma sanh: ");
    cout << "  Loai su kien: 1. Tieu cuoi (min 50 khach)  2. Hoi nghi (dat truoc 3 ngay)  3. Sinh nhat (max 100 khach)\n";
    int loai = docSoNguyen("  Chon loai su kien: ");
    string tenSK = docChuoi("  Ten su kien: ");
    int soKhach = docSoNguyen("  So luong khach: ");
    ThoiDiem bd = docThoiDiem("  Gio bat dau");
    ThoiDiem kt = docThoiDiem("  Gio ket thuc");

    BookingRequest* bk = ht.taoYeuCauDatLich(maSanh, loai, tenSK, soKhach, bd, kt);
    cout << "\n  Tao yeu cau dat lich THANH CONG (trang thai Pending):\n";
    bk->hienThi();
}

// Hủy lịch đã duyệt (FR-08)
void huyBookingUI(HeThongDatLich& ht) {
    lietKeTheoTrangThai(ht, APPROVED);
    string ma = docChuoi("  Ma booking can HUY: ");
    ht.huyLichDaDuyet(ma);
    cout << "  Da HUY booking " << ma << ". Khung gio duoc giai phong.\n";
}

// Đổi lịch (FR-09)
void doiLichUI(HeThongDatLich& ht) {
    cout << "  (Co the doi lich booking o trang thai Pending hoac Approved)\n";
    bool co = false;
    for (BookingRequest* b : ht.getDanhSachBooking()) {
        if (b->getTrangThai() == PENDING || b->getTrangThai() == APPROVED) {
            b->hienThi();
            co = true;
        }
    }
    if (!co) cout << "  (Khong co booking nao co the doi lich)\n";
    string ma = docChuoi("  Ma booking can DOI: ");
    ThoiDiem bd = docThoiDiem("  Gio bat dau moi");
    ThoiDiem kt = docThoiDiem("  Gio ket thuc moi");
    ht.doiLich(ma, bd, kt);
    cout << "  Da doi lich booking " << ma << " sang khung gio moi (giu nguyen ma booking):\n";
    for (BookingRequest* b : ht.getDanhSachBooking()) {
        if (b->getMaBooking() == ma) {
            b->hienThi();
            break;
        }
    }
}

// ================== Menu NGUOI YEU CAU ==================
void menuNguoiYeuCau(HeThongDatLich& ht) {
    while (true) {
        xoaManHinh(); // bắt đầu một màn hình mới: xóa nội dung màn hình cũ
        cout << "=========== NGUOI YEU CAU (Khach hang / Nhan vien tiep nhan) ===========\n"
             << "  1. Xem danh sach sanh\n"
             << "  2. Xem lich trong cua sanh theo ngay (FR-03)\n"
             << "  3. Tao yeu cau dat lich (FR-04)\n"
             << "  4. Huy lich da duyet (FR-08)\n"
             << "  5. Doi lich - reschedule (FR-09)\n"
             << "  6. Tra cuu danh sach lich (FR-10)\n"
             << "  0. Quay lai menu chinh\n";
        int chon = docSoNguyen("  Lua chon: ");
        if (chon == 0) return; // thoát menu: không dừng màn hình

        try {
            // Mỗi chức năng là một màn hình riêng: xóa màn hình cũ, hiển thị
            // nội dung mới rồi chờ người dùng nhấn Enter mới quay lại menu.
            switch (chon) {
                case 1: xoaManHinh(); ht.xemDanhSachSanh(); choNhanEnter(); break;
                case 2: xoaManHinh(); xemLichTrongUI(ht); choNhanEnter(); break;
                case 3: xoaManHinh(); taoBookingUI(ht); choNhanEnter(); break;
                case 4: xoaManHinh(); huyBookingUI(ht); choNhanEnter(); break;
                case 5: xoaManHinh(); doiLichUI(ht); choNhanEnter(); break;
                case 6: xoaManHinh(); traCuuLichUI(ht); choNhanEnter(); break;
                default:
                    cout << "  Lua chon khong hop le.\n";
                    choNhanEnter();
            }
        } catch (const exception& e) {
            inLoi(e);       // bắt mọi exception nghiệp vụ, hiển thị...
            choNhanEnter(); // ...rồi dừng màn hình cho người dùng đọc
        }
    }
}



// ================== FR-01: Quan ly danh muc sanh ==================
void menuQuanLySanh(HeThongDatLich& ht) {
    while (true) {
        xoaManHinh(); // bắt đầu một màn hình mới: xóa nội dung màn hình cũ
        cout << "---------------- QUAN LY DANH MUC SANH (FR-01) ----------------\n"
             << "  1. Xem danh sach sanh\n"
             << "  2. Them sanh moi\n"
             << "  3. Sua thong tin sanh\n"
             << "  4. Xoa sanh\n"
             << "  0. Quay lai\n";
        int chon = docSoNguyen("  Lua chon: ");
        if (chon == 0) return; // thoát menu: không dừng màn hình

        try {
            // Mỗi chức năng là một màn hình riêng: xóa màn hình cũ, hiển thị
            // nội dung mới rồi chờ người dùng nhấn Enter mới quay lại menu.
            switch (chon) {
                case 1: {
                    xoaManHinh();
                    ht.xemDanhSachSanh();
                    choNhanEnter();
                    break;
                }
                case 2: {
                    xoaManHinh();
                    string ma = docChuoi("  Ma sanh moi: ");
                    string ten = docChuoi("  Ten sanh: ");
                    int sucChua = docSoNguyen("  Suc chua toi da (so khach): ");
                    bool hd = docCoKhong("  Dang hoat dong? ");
                    ht.themSanh(ma, ten, sucChua, hd);
                    cout << "  Da them sanh " << ma << " thanh cong!\n";
                    choNhanEnter();
                    break;
                }
                case 3: {
                    xoaManHinh();
                    ht.xemDanhSachSanh();
                    string ma = docChuoi("  Ma sanh can sua: ");
                    string ten = docChuoi("  Ten moi: ");
                    int sucChua = docSoNguyen("  Suc chua moi (so khach): ");
                    bool hd = docCoKhong("  Dang hoat dong? ");
                    ht.suaSanh(ma, ten, sucChua, hd);
                    cout << "  Da cap nhat sanh " << ma << "!\n";
                    choNhanEnter();
                    break;
                }
                case 4: {
                    xoaManHinh();
                    ht.xemDanhSachSanh();
                    string ma = docChuoi("  Ma sanh can xoa: ");
                    ht.xoaSanh(ma);
                    cout << "  Da xoa sanh " << ma << "!\n";
                    choNhanEnter();
                    break;
                }
                default:
                    cout << "  Lua chon khong hop le.\n";
                    choNhanEnter();
            }
        } catch (const exception& e) {
            inLoi(e);       // hiển thị lỗi nghiệp vụ...
            choNhanEnter(); // ...rồi dừng màn hình cho người dùng đọc
        }
    }
}

// ================== Menu QUAN LY SANH ==================
void menuQuanLy(HeThongDatLich& ht) {
    while (true) {
        xoaManHinh(); // bắt đầu một màn hình mới: xóa nội dung màn hình cũ
        cout << "===================== QUAN LY SANH =====================\n"
             << "  1. Quan ly danh muc sanh (FR-01)\n"
             << "  2. Duyet yeu cau dat lich (FR-06)\n"
             << "  3. Tu choi yeu cau dat lich (FR-07)\n"
             << "  4. Xem toan bo booking / Tra cuu (FR-10)\n"
             << "  5. Xem lich trong cua sanh (FR-03)\n"
             << "  0. Quay lai menu chinh\n";
        int chon = docSoNguyen("  Lua chon: ");
        if (chon == 0) return; // thoát menu: không dừng màn hình

        try {
            // Mỗi chức năng là một màn hình riêng: xóa màn hình cũ, hiển thị
            // nội dung mới rồi chờ người dùng nhấn Enter mới quay lại menu.
            switch (chon) {
                // Menu con tự lo việc xóa màn hình và dừng màn hình của nó
                case 1: menuQuanLySanh(ht); break;
                case 2: {
                    xoaManHinh();
                    lietKeTheoTrangThai(ht, PENDING);
                    string ma = docChuoi("  Ma booking can DUYET: ");
                    ht.duyetYeuCau(ma);
                    cout << "  Da DUYET booking " << ma << " (trang thai: Approved).\n";
                    choNhanEnter();
                    break;
                }
                case 3: {
                    xoaManHinh();
                    lietKeTheoTrangThai(ht, PENDING);
                    string ma = docChuoi("  Ma booking can TU CHOI: ");
                    string lyDo = docChuoi("  Ly do tu choi: ");
                    ht.tuChoiYeuCau(ma, lyDo);
                    cout << "  Da TU CHOI booking " << ma << ".\n";
                    choNhanEnter();
                    break;
                }
                case 4: {
                    xoaManHinh();
                    cout << "  Toan bo booking trong he thong:\n";
                    if (ht.getDanhSachBooking().empty()) cout << "  (Danh sach rong)\n";
                    for (BookingRequest* b : ht.getDanhSachBooking()) b->hienThi();
                    traCuuLichUI(ht);
                    choNhanEnter();
                    break;
                }
                case 5: {
                    xoaManHinh();
                    xemLichTrongUI(ht);
                    choNhanEnter();
                    break;
                }
                default:
                    cout << "  Lua chon khong hop le.\n";
                    choNhanEnter();
            }
        } catch (const exception& e) {
            inLoi(e);       // hiển thị lỗi nghiệp vụ...
            choNhanEnter(); // ...rồi dừng màn hình cho người dùng đọc
        }
    }
}

// ================== Chuong trinh chinh ==================
int main() {
#ifdef _WIN32
    system("chcp 65001 > nul"); // hiển thị tiếng Việt đúng trên console Windows
#endif
    HeThongDatLich ht;
    ht.docDuLieuTuFile();   // nạp dữ liệu đã lưu trong file txt (nếu có)
    ht.khoiTaoDuLieuMau();  // chỉ sinh dữ liệu mẫu khi hệ thống còn rỗng (lần chạy đầu)

    try {
        while (true) {
            xoaManHinh(); // về menu chính = màn hình mới, xóa nội dung cũ
            cout << "=============================================================\n"
                 << "   HE THONG DAT LICH SU KIEN - Trung tam Tiec cuoi & Su kien\n"
                 << "   Do an mon Lap trinh Huong doi tuong (OOP)\n"
                 << "=============================================================\n"
                 << "\n======================== MENU CHINH ========================\n"
                 << "  Chon vai tro:\n"
                 << "  1. Nguoi yeu cau (khach hang / nhan vien tiep nhan)\n"
                 << "  2. Quan ly sanh\n"
                 << "  0. Thoat chuong trinh\n";
            int chon = docSoNguyen("  Lua chon: ");
            if (chon == 0) break;
            switch (chon) {
                // Các menu con tự lo việc xóa màn hình và dừng màn hình của nó
                case 1: menuNguoiYeuCau(ht); break;
                case 2: menuQuanLy(ht); break;
                default:
                    cout << "  Lua chon khong hop le.\n";
                    choNhanEnter();
            }
        }
    } catch (const exception& e) {
        // Lỗi trong suốt vòng đời chương trình (VD: EOF khi nhập)
        inLoi(e);
    }

    cout << "\n  Cam on da su dung he thong. Tam biet!\n";
    return 0;
}
