#include "Utils.h"
#include "Exceptions.h"
#include <ctime>
#include <cstdlib>

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



