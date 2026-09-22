#include "Utils.h"
#include "Exceptions.h"
#include "Hall.h"
#include "Event.h"
#include "WeddingEvent.h"
#include "ConferenceEvent.h"
#include "BirthdayEvent.h"
#include "BookingRequest.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unordered_map>
#ifdef _WIN32
#include <direct.h>   // _mkdir tren Windows
#else
#include <sys/stat.h> // mkdir tren Linux/macOS
#endif

using namespace std;

// ================== ThoiDiem ==================
ThoiDiem::ThoiDiem() : ngay(1), thang(1), nam(2026), gio(0), phut(0) {}

ThoiDiem::ThoiDiem(int ngay, int thang, int nam, int gio, int phut)
    : ngay(ngay), thang(thang), nam(nam), gio(gio), phut(phut) {}

// Số ngày từ 01/01/năm 1 đến đầu năm `nam` (hàm nội bộ của Utils)
static long long soNgayTruocNam(int nam) {
    long long y = nam - 1;
    return y * 365 + y / 4 - y / 100 + y / 400;
}

// Quy đổi toàn bộ mốc thời gian thành số phút tuyệt đối để dễ so sánh
long long ThoiDiem::phutTuyetDoi() const {
    static const int snThang[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    long long d = soNgayTruocNam(nam);
    for (int m = 1; m < thang; ++m) {
        d += snThang[m - 1];
        if (m == 2 && laNamNhuan(nam)) ++d;
    }
    d += ngay;
    return d * 1440LL + gio * 60LL + phut;
}

bool ThoiDiem::operator<(const ThoiDiem& khac) const {
    return phutTuyetDoi() < khac.phutTuyetDoi();
}

bool ThoiDiem::operator>=(const ThoiDiem& khac) const {
    return !(*this < khac);
}

string ThoiDiem::toString() const {
    ostringstream os;
    os << setfill('0') << setw(2) << ngay << "/" << setw(2) << thang << "/" << nam
       << " " << setw(2) << gio << ":" << setw(2) << phut;
    return os.str();
}

// ================== Tiện ích thời gian ==================
bool laNamNhuan(int nam) {
    return (nam % 4 == 0 && nam % 100 != 0) || (nam % 400 == 0);
}

int soNgayTrongThang(int thang, int nam) {
    static const int sn[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (thang < 1 || thang > 12) return 0;
    if (thang == 2 && laNamNhuan(nam)) return 29;
    return sn[thang - 1];
}

bool laThoiDiemHopLe(const ThoiDiem& t) {
    if (t.thang < 1 || t.thang > 12) return false;
    if (t.ngay < 1 || t.ngay > soNgayTrongThang(t.thang, t.nam)) return false;
    if (t.gio < 0 || t.gio > 23) return false;
    if (t.phut < 0 || t.phut > 59) return false;
    return true;
}

// Thời điểm hiện tại của hệ thống (đọc từ đồng hồ máy tính)
ThoiDiem hienTai() {
    time_t bayGio = time(nullptr);
    tm x;
#ifdef _WIN32
    localtime_s(&x, &bayGio);
#else
    localtime_r(&bayGio, &x);
#endif
    return ThoiDiem(x.tm_mday, x.tm_mon + 1, x.tm_year + 1900, x.tm_hour, x.tm_min);
}
// Khôi phục ThoiDiem từ số ngày tuyệt đối (nghịch đảo của phutTuyetDoi)
static ThoiDiem tuNgayTuyetDoi(long long d, int gio, int phut) {
    static const int snThang[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int nam = 1;
    while (soNgayTruocNam(nam + 1) < d) ++nam; // tìm năm chứa ngày d
    long long conLai = d - soNgayTruocNam(nam); // ngày thứ `conLai` trong năm
    int thang = 1;
    while (true) {
        int sn = snThang[thang - 1] + ((thang == 2 && laNamNhuan(nam)) ? 1 : 0);
        if (conLai < sn) break;
        conLai -= sn;
        ++thang;
    }
    return ThoiDiem((int)conLai, thang, nam, gio, phut);
}

// Cộng/trừ số phút vào một mốc thời gian (hỗ trợ cả số âm — dùng cho
// việc "lùi về trước" thời gian chuẩn bị của sự kiện)
ThoiDiem congPhut(const ThoiDiem& t, long long soPhut) {
    long long m = t.phutTuyetDoi() + soPhut;
    long long d = m / 1440;
    int rem = (int)(m % 1440);
    return tuNgayTuyetDoi(d, rem / 60, rem % 60);
}

// Hai khung giờ nửa mở [bd..kt) có giao nhau không?
// => chỉ cần kiểm tra: kt của khung này phải "sau" bd của khung kia.
bool haiKhungGiaoNhau(const ThoiDiem& bd1, const ThoiDiem& kt1,
                      const ThoiDiem& bd2, const ThoiDiem& kt2) {
    return bd1 < kt2 && bd2 < kt1;
}

// ================== Tiện ích nhập liệu ==================
string docChuoi(const string& tieuDe) {
    while (true) {
        if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
        cout << tieuDe;
        string s;
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "  Khong duoc de trong, vui long nhap lai!\n";
    }
}

int docSoNguyen(const string& tieuDe) {
    while (true) {
        if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
        cout << tieuDe;
        string s;
        getline(cin, s);
        try {
            return stoi(s);
        } catch (const exception&) {
            cout << "  Gia tri khong hop le, vui long nhap mot so nguyen!\n";
        }
    }
}
// Phân tích chuỗi theo định dạng dd/mm/yyyy hh:mm
// Sai định dạng => ném InvalidDateException (FR-03, FR-04)
ThoiDiem phanTichThoiDiem(const string& chuoi) {
    int ngay, thang, nam, gio, phut;
    char c1, c2, c3;
    stringstream ss(chuoi);
    ss >> ngay >> c1 >> thang >> c2 >> nam >> gio >> c3 >> phut;
    if (!ss) {
        throw InvalidDateException("Sai dinh dang. Vui long dung dd/mm/yyyy hh:mm (VD: 20/10/2026 17:30)");
    }
    ThoiDiem t(ngay, thang, nam, gio, phut);
    if (!laThoiDiemHopLe(t)) {
        throw InvalidDateException("Ngay/gio khong ton tai: " + t.toString());
    }
    return t;
}

// Nhập thời điểm bắt đầu/kết thúc sự kiện — bắt buộc phải ở tương lai
ThoiDiem docThoiDiem(const string& tieuDe) {
    while (true) {
        if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
        cout << tieuDe << " (dinh dang dd/mm/yyyy hh:mm): ";
        string s;
        getline(cin, s);
        try {
            ThoiDiem t = phanTichThoiDiem(s);
            if (t < hienTai()) {
                throw InvalidDateException("Thoi diem da trong qua khu, hay chon thoi diem trong tuong lai");
            }
            return t;
        } catch (const InvalidDateException& e) {
            cout << "  Loi: " << e.what() << ". Vui long nhap lai.\n";
        }
    }
}

// Nhập một ngày (không cần giờ) — dùng cho tra cứu/xem lịch trống,
// cho phép cả ngày trong quá khứ
void docNgay(const string& tieuDe, int& ngay, int& thang, int& nam) {
    while (true) {
        if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
        cout << tieuDe << " (dinh dang dd/mm/yyyy): ";
        string s;
        getline(cin, s);
        char c1, c2;
        stringstream ss(s);
        ss >> ngay >> c1 >> thang >> c2 >> nam;
        if (!ss) {
            cout << "  Loi: sai dinh dang. Vui long dung dd/mm/yyyy (VD: 20/10/2026).\n";
            continue;
        }
        ThoiDiem t(ngay, thang, nam, 0, 0);
        if (!laThoiDiemHopLe(t)) {
            cout << "  Loi: ngay khong ton tai. Vui long nhap lai.\n";
            continue;
        }
        return;
    }
}

// Nhập lựa chọn kiểu có/không (y/n)
bool docCoKhong(const string& tieuDe) {
    while (true) {
        if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
        cout << tieuDe << " (y/n): ";
        string s;
        getline(cin, s);
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        cout << "  Vui long chi nhap y hoac n.\n";
    }
}
// ================== Tiện ích in bảng trên console ==================
// Đếm số ký tự hiển thị thực tế: bỏ qua các byte nối tiếp của UTF-8
// (0x80..0xBF) để một chữ có dấu được tính là 1 cột, giúp bảng thẳng hàng.
int doRongHienThi(const string& s) {
    int rong = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if ((c & 0xC0) != 0x80) ++rong;
    }
    return rong;
}

// Canh chuỗi trong một ô có độ rộng cho trước (mặc định canh trái)
string canLe(const string& s, int rong, bool canPhai) {
    int thieu = rong - doRongHienThi(s);
    if (thieu <= 0) return s;          // ô quá hẹp: giữ nguyên, không cắt
    string dem(thieu, ' ');
    return canPhai ? dem + s : s + dem;
}

// Đường kẻ ngang của bảng, ví dụ: +------+------------+---+
string duongKeBang(const vector<int>& doRong) {
    string ke = "+";
    for (int r : doRong) ke += string(r + 2, '-') + "+";
    return ke;
}

// In một dòng dữ liệu: | o1 | o2 | ... (mỗi ô cách lề 1 khoảng trắng)
void inDongBang(const vector<string>& o, const vector<int>& doRong,
                const vector<bool>& canPhai) {
    cout << "|";
    for (size_t i = 0; i < o.size(); ++i) {
        bool phai = i < canPhai.size() && canPhai[i];
        cout << " " << canLe(o[i], doRong[i], phai) << " |";
    }
    cout << "\n";
}

// ================== Tiện ích màn hình console ==================
// Xóa toàn bộ nội dung màn hình (lệnh khác nhau giữa Windows và Unix)
void xoaManHinh() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Dừng màn hình chờ người dùng nhấn Enter rồi mới trả quyền cho menu gọi nó.
// Nhờ vậy danh sách/kết quả vừa in vẫn còn trên màn hình để đọc.
void choNhanEnter(const string& thongBao) {
    if (cin.eof()) throw runtime_error("Ket thuc dau vao (EOF)");
    cout << thongBao;
    string boQua;
    getline(cin, boQua);
}

// ================== Luu / nap du lieu ra file txt ==================
// Toan bo du lieu (sanh + booking) duoc luu thanh file txt trong thu muc
// `data` nam canh chuong trinh, nho do du lieu KHONG bi mat khi chay lai.
// Duong dan la tuong doi so voi thu muc lam viec cua tien trinh.
static const string DU_LIEU_FOLDER = "data";
static const string FILE_SANH = DU_LIEU_FOLDER + "/sanh.txt";
static const string FILE_BOOKING = DU_LIEU_FOLDER + "/booking.txt";

// ----- Dong tieu de (header) cua moi file du lieu -----
// Header duoc ghi o dau file de nguoi doc (mang nguoi cham, mo bang Excel...)
// de hieu cau truc du lieu; khi doc, cac dong nay duoc bo qua nen khong anh
// huong den viec nap du lieu. File cu khong co header van doc binh thuong
// (kiem tra bo qua la "nho" nen khong mat du lieu).
static const string TIEU_DE_SANH =
    "MaSanh|TenSanh|SucChua|DangHoatDong";
static const string TIEU_DE_BOOKING =
    "MaBooking|MaSanh|LoaiSuKien|MaSuKien|TenSuKien|SoKhach|"
    "GioBatDau|GioKetThuc|TrangThai|LyDoTuChoi|NgayTao";

// Dong chu thich bat dau bang '#', dong tieu de trung khop voi chuoi header
static bool laDongChuThich(const string& dong) {
    return !dong.empty() && dong[0] == '#';
}
static bool laDongTieuDe(const string& dong, const string& tieuDe) {
    return dong == tieuDe;
}

// Tao thu muc du lieu neu chua co (khac nhau giua Windows va Linux/macOS).
static void khoiPhucThuMucDuLieu() {
#ifdef _WIN32
    _mkdir(DU_LIEU_FOLDER.c_str());        // <direct.h>
#else
    mkdir(DU_LIEU_FOLDER.c_str(), 0777);   // <sys/stat.h>
#endif
}

// Tach mot dong trong file txt thanh cac truong theo ky tu phan cach '|'
static vector<string> tachTruong(const string& dong) {
    vector<string> truong;
    stringstream ss(dong);
    string t;
    while (getline(ss, t, '|')) truong.push_back(t);
    return truong;
}

// Lam sach chuoi truoc khi ghi: thay ky tu phan cach '|' va ky tu xuong dong
// (do nguoi dung co the nhap) bang khoang trang de khong pha cau truc file.
static string lamSach(const string& s) {
    string kq = s;
    for (size_t i = 0; i < kq.size(); ++i) {
        if (kq[i] == '|' || kq[i] == '\n' || kq[i] == '\r') kq[i] = ' ';
    }
    return kq;
}

// Doc so nguyen an toan: tra ve gia tri mac dinh neu chuoi khong hop le
// (giup bo qua du lieu hong ma khong lam chet chuong trinh).
static int docSoAnToan(const string& s, int macDinh) {
    try {
        return stoi(s);
    } catch (const exception&) {
        return macDinh;
    }
}

// ---- Ghi file sanh: moi dong la mot sanh ----
// Dinh dang: maSanh|ten|sucChua|dangHoatDong(0/1)
// Dau file co header (chu thich '#' + dong ten cot) de de doc.
static void ghiFileSanh(const vector<Hall*>& danhSachSanh) {
    ofstream fout(FILE_SANH.c_str(), ios::trunc);
    if (!fout) {
        cerr << "[LOI] Khong the mo file " << FILE_SANH << " de ghi du lieu.\n";
        return;
    }
    // ----- Header cua file -----
    fout << "# ===== DU LIEU SANH - He thong dat lich su kien =====\n"
         << "# Dinh dang moi dong du lieu: MaSanh|TenSanh|SucChua|DangHoatDong\n"
         << "# DangHoatDong: 1 = dang hoat dong, 0 = ngung hoat dong\n"
         << TIEU_DE_SANH << "\n";
    for (size_t i = 0; i < danhSachSanh.size(); ++i) {
        const Hall* h = danhSachSanh[i];
        fout << lamSach(h->getMaSanh()) << "|"
             << lamSach(h->getTen()) << "|"
             << h->getSucChua() << "|"
             << (h->getDangHoatDong() ? 1 : 0)
             << "\n";
    }
}

// ---- Doc file sanh ----
static void docFileSanh(vector<Hall*>& danhSachSanh) {
    ifstream fin(FILE_SANH.c_str());
    if (!fin) return; // chua co file du lieu -> coi nhu rong (khong phai loi)

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (laDongChuThich(line)) continue;               // bo qua chu thich '#'
        if (laDongTieuDe(line, TIEU_DE_SANH)) continue;   // bo qua dong header
        vector<string> t = tachTruong(line);
        if (t.size() < 4) continue; // dong thieu truong -> bo qua
        const string& maSanh = t[0];
        const string& ten = t[1];
        int sucChua = docSoAnToan(t[2], 0);
        bool dangHoatDong = (docSoAnToan(t[3], 0) != 0);
        if (sucChua <= 0) {
            cerr << "[LOI] Bo qua sanh \"" << maSanh << "\": suc chua khong hop le.\n";
            continue;
        }
        danhSachSanh.push_back(new Hall(maSanh, ten, sucChua, dangHoatDong));
    }
}

// ---- Ghi file booking: moi dong la mot booking kem du lieu su kien con ----
// Dinh dang (11 truong, phan cach '|'):
//   maBooking|maSanh|loaiSuKien|maSuKien|tenSuKien|soKhach|
//   gioBatDau|gioKetThuc|trangThai|lyDoTuChoi|ngayTao
// Trong do:
//   loaiSuKien : 1 = Tiec cuoi, 2 = Hoi nghi, 3 = Sinh nhat
//                (lay qua ham ao maLoai() -> the hien tinh DA HINH)
//   trangThai  : 0 = Pending, 1 = Approved, 2 = Rejected, 3 = Cancelled
//   thoi diem  : dd/mm/yyyy hh:mm (khop voi ThoiDiem::toString)
// Dau file co header (chu thich '#' + dong ten cot) de de doc.
static void ghiFileBooking(const vector<BookingRequest*>& danhSachBooking) {
    ofstream fout(FILE_BOOKING.c_str(), ios::trunc);
    if (!fout) {
        cerr << "[LOI] Khong the mo file " << FILE_BOOKING << " de ghi du lieu.\n";
        return;
    }
    // ----- Header cua file -----
    fout << "# ===== DU LIEU BOOKING - He thong dat lich su kien =====\n"
         << "# Dinh dang moi dong du lieu (11 truong, phan cach '|'):\n"
         << "# " << TIEU_DE_BOOKING << "\n"
         << "# LoaiSuKien: 1 = Tiec cuoi, 2 = Hoi nghi, 3 = Sinh nhat\n"
         << "# TrangThai : 0 = Pending, 1 = Approved, 2 = Rejected, 3 = Cancelled\n"
         << "# Thoi diem ghi dang: dd/mm/yyyy hh:mm\n"
         << TIEU_DE_BOOKING << "\n";
    for (size_t i = 0; i < danhSachBooking.size(); ++i) {
        const BookingRequest* b = danhSachBooking[i];
        const Hall* h = b->getSanh();
        const Event* e = b->getSuKien();
        if (h == nullptr || e == nullptr) continue; // du lieu khong hop le -> bo qua
        fout << lamSach(b->getMaBooking()) << "|"
             << lamSach(h->getMaSanh()) << "|"
             << e->maLoai() << "|"
             << lamSach(e->getMaSuKien()) << "|"
             << lamSach(e->getTenSuKien()) << "|"
             << e->getSoKhach() << "|"
             << e->getGioBatDau().toString() << "|"
             << e->getGioKetThuc().toString() << "|"
             << (int)b->getTrangThai() << "|"
             << lamSach(b->getLyDoTuChoi()) << "|"
             << b->getNgayTao().toString()
             << "\n";
    }
}

// ---- Doc file booking ----
// Khoi phuc dung lop con cua Event nho truong loaiSuKien da luu.
// Moi dong loi duoc bo qua kem canh bao, khong lam chet chuong trinh.
static void docFileBooking(vector<BookingRequest*>& danhSachBooking,
                           const vector<Hall*>& danhSachSanh) {
    ifstream fin(FILE_BOOKING.c_str());
    if (!fin) return; // chua co file du lieu -> coi nhu rong (khong phai loi)

    // Bang tra ma sanh -> con tro Hall de gan lai quan he has-a
    unordered_map<string, Hall*> mapSanh;
    for (size_t i = 0; i < danhSachSanh.size(); ++i) {
        mapSanh[danhSachSanh[i]->getMaSanh()] = danhSachSanh[i];
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (laDongChuThich(line)) continue;                 // bo qua chu thich '#'
        if (laDongTieuDe(line, TIEU_DE_BOOKING)) continue;  // bo qua dong header
        vector<string> t = tachTruong(line);
        if (t.size() < 11) {
            cerr << "[LOI] Bo qua mot dong booking sai dinh dang (thieu truong).\n";
            continue;
        }
        const string& maBooking = t[0];
        const string& maSanh = t[1];
        int loaiSuKien = docSoAnToan(t[2], 0);
        const string& maSuKien = t[3];
        const string& tenSuKien = t[4];
        int soKhach = docSoAnToan(t[5], 0);

        // Gan lai sanh cho booking
        Hall* sanh = nullptr;
        unordered_map<string, Hall*>::iterator it = mapSanh.find(maSanh);
        if (it != mapSanh.end()) sanh = it->second;
        if (sanh == nullptr) {
            cerr << "[LOI] Bo qua booking \"" << maBooking
                 << "\": khong tim thay sanh \"" << maSanh << "\".\n";
            continue;
        }

        // Tao dung lop con cua Event theo ma loai da luu
        Event* suKien = nullptr;
        try {
            ThoiDiem bd = phanTichThoiDiem(t[6]);
            ThoiDiem kt = phanTichThoiDiem(t[7]);
            switch (loaiSuKien) {
                case 1: suKien = new WeddingEvent(maSuKien, tenSuKien, soKhach, bd, kt); break;
                case 2: suKien = new ConferenceEvent(maSuKien, tenSuKien, soKhach, bd, kt); break;
                case 3: suKien = new BirthdayEvent(maSuKien, tenSuKien, soKhach, bd, kt); break;
                default:
                    cerr << "[LOI] Bo qua booking \"" << maBooking
                         << "\": loai su kien khong hop le (" << loaiSuKien << ").\n";
                    continue;
            }
        } catch (const exception& e) {
            cerr << "[LOI] Bo qua booking \"" << maBooking
                 << "\": thoi diem khong hop le (" << e.what() << ").\n";
            if (suKien != nullptr) delete suKien;
            continue;
        }

        BookingRequest* b = new BookingRequest(maBooking, sanh, suKien);
        int tt = docSoAnToan(t[8], 0);
        if (tt < 0 || tt > 3) tt = 0; // trang thai la -> coi nhu Pending
        b->setTrangThai((TrangThai)tt);
        b->setLyDoTuChoi(t[9]);
        try {
            b->setNgayTao(phanTichThoiDiem(t[10]));
        } catch (const exception&) {
            b->setNgayTao(hienTai()); // ngay tao hong -> lay thoi diem hien tai
        }
        danhSachBooking.push_back(b);
    }
}

// ---- Luu toan bo du lieu he thong ra file txt ----
void luuHeThongRaFile(const vector<Hall*>& danhSachSanh,
                      const vector<BookingRequest*>& danhSachBooking) {
    khoiPhucThuMucDuLieu();
    ghiFileSanh(danhSachSanh);
    ghiFileBooking(danhSachBooking);
}

// ---- Nap toan bo du lieu he thong tu file txt ----
void napHeThongTuFile(vector<Hall*>& danhSachSanh,
                      vector<BookingRequest*>& danhSachBooking) {
    khoiPhucThuMucDuLieu();
    docFileSanh(danhSachSanh);                     // doc sanh truoc
    docFileBooking(danhSachBooking, danhSachSanh); // roi gan booking vao sanh
}
