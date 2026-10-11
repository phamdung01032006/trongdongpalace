#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

// =====================================================================
// Vai trò của người dùng trong hệ thống:
//   1: NGUOI_YEU_CAU (Khách hàng / Nhân viên tiếp nhận)
//   2: QUAN_LY (Quản lý sảnh / Quản trị viên)
// =====================================================================
enum VaiTro {
    VAI_TRO_NGUOI_YEU_CAU = 1,
    VAI_TRO_QUAN_LY = 2
};

string vaiTroThanhChu(VaiTro vt);
string vaiTroThanhMa(VaiTro vt);
VaiTro phanTichVaiTro(const string& str);

// =====================================================================
// Lớp User: biểu diễn một tài khoản người dùng trong hệ thống.
// Đóng gói thông tin tài khoản, mật khẩu, vai trò và họ tên.
// =====================================================================
class User {
private:
    string tenDangNhap;
    string matKhau;
    VaiTro vaiTro;
    string hoTen;

public:
    User();
    User(const string& tenDangNhap, const string& matKhau,
         VaiTro vaiTro, const string& hoTen = "");

    const string& getTenDangNhap() const;
    const string& getMatKhau() const;
    VaiTro getVaiTro() const;
    const string& getHoTen() const;
    string getTenVaiTro() const;

    void setTenDangNhap(const string& tenDangNhap);
    void setMatKhau(const string& matKhau);
    void setVaiTro(VaiTro vaiTro);
    void setHoTen(const string& hoTen);

    bool kiemTraMatKhau(const string& matKhauKiemTra) const;
};

#endif // USER_H

