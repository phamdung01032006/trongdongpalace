#ifndef HALL_H
#define HALL_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

// =====================================================================
// Lớp cụ thể mô tả một sảnh tiệc (FR-01, Mục 6).
// Quan hệ: 1 Hall — nhiều BookingRequest.
// =====================================================================
class Hall {
private:
    string maSanh;
    string ten;
    int sucChua;      // sức chứa tối đa (số khách)
    bool dangHoatDong; // true: đang hoạt động, false: ngừng hoạt động

public:
    Hall(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong);

    string getMaSanh() const;
    string getTen() const;
    int getSucChua() const;
    bool getDangHoatDong() const;

    void setTen(const string& tenMoi);
    void setSucChua(int sucChuaMoi);
    void setDangHoatDong(bool hoatDong);

    // In một dòng của bảng danh mục sảnh, dùng độ rộng cột do bên gọi tính sẵn
    // (giúp mọi dòng thẳng hàng với tiêu đề và đường kẻ của bảng).
    void hienThi(const vector<int>& doRong) const;
};

#endif // HALL_H
