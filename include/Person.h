#pragma once

// ==================== TINH KE THUA ====================
// Person la lop CO SO (base class) chua cac thong tin chung cua mot con nguoi:
// ho ten, so dien thoai, email, dia chi.
//
// Person la lop TRUU TUONG (abstract) vi co ham thuan ao (pure virtual)
// roleName(). Ta KHONG the tao doi tuong Person truc tiep, ma phai tao
// Customer hoac User (hai lop con).
//
// ==================== TINH DA HINH ====================
// Con tro/reference kieu Person* co the tro toi Customer hoac User.
// Khi goi p->roleName() hay p->describe(), C++ se goi dung phien ban cua
// lop con (late binding) nho tu khoa virtual.

#include <string>

using namespace std;

class Person {
public:
    // Mot ham tao duy nhat voi tham so mac dinh: vua tao duoc Person(),
    // vua tao duoc Person(hoTen, sdt, email, diaChi) theo dung yêu cầu.
    Person(string fullName = "", string phone = "", string email = "",
           string address = "");
    virtual ~Person() = default;

    const string& get_fullName() const { return fullName_; }
    const string& get_phone() const { return phone_; }
    const string& get_email() const { return email_; }
    const string& get_address() const { return address_; }

    void set_fullName(const string& value) { fullName_ = value; }
    void set_phone(const string& value) { phone_ = value; }
    void set_email(const string& value) { email_ = value; }
    void set_address(const string& value) { address_ = value; }

    // Ham thuan ao: lop con BAT BUOC phai cai dat.
    virtual string roleName() const = 0;

    // Ham ao co cai dat san: lop con co the ghi de (override).
    virtual string describe() const;

protected:
    string fullName_;
    string phone_;
    string email_;
    string address_;
};
