#include "Hall.h"
#include "Utils.h"

using namespace std;

Hall::Hall(const string& maSanh, const string& ten, int sucChua, bool dangHoatDong)
    : maSanh(maSanh), ten(ten), sucChua(sucChua), dangHoatDong(dangHoatDong) {}

string Hall::getMaSanh() const { return maSanh; }
string Hall::getTen() const { return ten; }
int Hall::getSucChua() const { return sucChua; }
bool Hall::getDangHoatDong() const { return dangHoatDong; }

void Hall::setTen(const string& tenMoi) { ten = tenMoi; }
void Hall::setSucChua(int sucChuaMoi) { sucChua = sucChuaMoi; }
void Hall::setDangHoatDong(bool hoatDong) { dangHoatDong = hoatDong; }

void Hall::hienThi(const vector<int>& doRong) const {
    vector<string> o = {
        maSanh,
        ten,
        to_string(sucChua) + " khach",
        dangHoatDong ? "Dang hoat dong" : "Ngung hoat dong"
    };
    // Cột "Suc chua" canh phải cho số thẳng cột, các cột còn lại canh trái
    inDongBang(o, doRong, {false, false, true, false});
}
