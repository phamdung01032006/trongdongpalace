#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// Khai báo trước (forward declaration) để Utils.h dùng được tên lớp này
// mà không cần include Hall.h / BookingRequest.h (tránh include vòng).
class Hall;
class BookingRequest;

// =====================================================================
// Tập hợp các tiện ích dùng chung của hệ thống.
// Trong đó cấu trúc ThoiDiem hỗ trợ so sánh (<, >=) và cộng/trừ số phút,
// là nền tảng cho việc kiểm tra trùng lịch (FR-05).
// =====================================================================

// ---------- Cấu trúc mốc thời gian: ngày/tháng/năm + giờ:phút ----------
struct ThoiDiem {
    int ngay;
    int thang;
    int nam;
    int gio;
    int phut;

    ThoiDiem();
    ThoiDiem(int ngay, int thang, int nam, int gio, int phut);

    // Quy đổi thành số phút tuyệt đối (dùng để so sánh / cộng trừ)
    long long phutTuyetDoi() const;

    bool operator<(const ThoiDiem& khac) const;
    bool operator>=(const ThoiDiem& khac) const;

    string toString() const; // "dd/mm/yyyy hh:mm"
};

// ---------- Tiện ích về thời gian ----------
bool laNamNhuan(int nam);
int soNgayTrongThang(int thang, int nam);
bool laThoiDiemHopLe(const ThoiDiem& t);
ThoiDiem hienTai();                                  // thời điểm hiện tại của hệ thống
ThoiDiem congPhut(const ThoiDiem& t, long long soPhut);
// Hai khung giờ [bd1..kt1) và [bd2..kt2) có giao nhau không (FR-05)
bool haiKhungGiaoNhau(const ThoiDiem& bd1, const ThoiDiem& kt1,
                      const ThoiDiem& bd2, const ThoiDiem& kt2);

// ---------- Tiện ích nhập liệu từ console ----------
string docChuoi(const string& tieuDe);
int docSoNguyen(const string& tieuDe);
ThoiDiem phanTichThoiDiem(const string& chuoi);      // ném InvalidDateException nếu sai
ThoiDiem docThoiDiem(const string& tieuDe);          // nhập + kiểm tra (phải ở tương lai)
void docNgay(const string& tieuDe, int& ngay, int& thang, int& nam);
bool docCoKhong(const string& tieuDe);               // nhập y/n

// ---------- Tiện ích in bảng trên console ----------
// Bộ hàm nhỏ giúp dựng các bảng dạng | ... | ... | luôn thẳng hàng.
// Cột được canh theo "độ rộng hiển thị" (đếm ký tự thực tế) nên không bị
// lệch khi dữ liệu có dấu tiếng Việt (UTF-8) hoặc dài ngắn khác nhau.
int doRongHienThi(const string& s);                          // số cột ký tự hiển thị
string canLe(const string& s, int rong, bool canPhai = false); // bù khoảng trắng cho đủ ô
string duongKeBang(const vector<int>& doRong);               // "+------+-----+"
void inDongBang(const vector<string>& o, const vector<int>& doRong,
                const vector<bool>& canPhai = vector<bool>()); // "| ... | ... |"

// ---- Lưu / nạp dữ liệu ra file txt (dữ liệu không mất khi chạy lại) ----
// Tên hàm có tiền tố "luuHeThong"/"napHeThong" để KHÔNG trùng với các method
// cùng tên của lớp HeThongDatLich (tránh lỗi che khuất tên - name hiding).
void luuHeThongRaFile(const vector<Hall*>& danhSachSanh,
                      const vector<BookingRequest*>& danhSachBooking);
void napHeThongTuFile(vector<Hall*>& danhSachSanh,
                      vector<BookingRequest*>& danhSachBooking);

// ---------- Tiện ích màn hình console ----------
// Xóa toàn bộ nội dung màn hình — dùng mỗi khi chuyển sang màn hình/menu mới
// để trên màn hình chỉ còn nội dung của màn hình hiện tại.
void xoaManHinh();
// Dừng màn hình để người dùng đọc kết quả, chỉ quay lại menu sau khi nhấn Enter
void choNhanEnter(const string& thongBao = "  Nhan Enter de quay lai...");

#endif // UTILS_H
