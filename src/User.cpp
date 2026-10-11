#include "User.h"
#include <cctype>
#include <stdexcept>

using namespace std;

string vaiTroThanhChu(VaiTro vt) {
    switch (vt) {
        case VAI_TRO_QUAN_LY:
            return "Quan ly sanh";
        case VAI_TRO_NGUOI_YEU_CAU:
            return "Nguoi yeu cau";
        default:
            return "Khong xac dinh";
    }
}

string vaiTroThanhMa(VaiTro vt) {
    switch (vt) {
        case VAI_TRO_QUAN_LY:
            return "QUAN_LY";
        case VAI_TRO_NGUOI_YEU_CAU:
            return "NGUOI_YEU_CAU";
        default:
            return "KHAC";
    }
}

VaiTro phanTichVaiTro(const string& str) {
    string s = str;
    for (char& c : s) c = toupper((unsigned char)c);
    if (s == "2" || s == "QUAN_LY" || s == "ADMIN" || s == "QUAN_TRI" || s == "QUANLY" || s == "QUAN_LY_SANH") {
        return VAI_TRO_QUAN_LY;
    }
    if (s == "1" || s == "NGUOI_YEU_CAU" || s == "KHACH_HANG" || s == "NHAN_VIEN" || s == "NGUOIYEUCAU" || s == "USER") {
        return VAI_TRO_NGUOI_YEU_CAU;
    }
    throw invalid_argument("Vai tro khong hop le: " + str);
}

User::User() : vaiTro(VAI_TRO_NGUOI_YEU_CAU) {}

User::User(const string& tenDangNhap, const string& matKhau,
           VaiTro vaiTro, const string& hoTen)
    : tenDangNhap(tenDangNhap), matKhau(matKhau), vaiTro(vaiTro), hoTen(hoTen) {}

const string& User::getTenDangNhap() const { return tenDangNhap; }
const string& User::getMatKhau() const { return matKhau; }
VaiTro User::getVaiTro() const { return vaiTro; }
const string& User::getHoTen() const { return hoTen; }
string User::getTenVaiTro() const { return vaiTroThanhChu(vaiTro); }

void User::setTenDangNhap(const string& tenDangNhap) { this->tenDangNhap = tenDangNhap; }
void User::setMatKhau(const string& matKhau) { this->matKhau = matKhau; }
void User::setVaiTro(VaiTro vaiTro) { this->vaiTro = vaiTro; }
void User::setHoTen(const string& hoTen) { this->hoTen = hoTen; }

bool User::kiemTraMatKhau(const string& matKhauKiemTra) const {
    return this->matKhau == matKhauKiemTra;
}

