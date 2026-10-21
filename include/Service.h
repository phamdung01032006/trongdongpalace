#pragma once

// ==================== TINH DA HINH ====================
// Service la lop truu tuong cho moi dich vu cua trung tam.
// Co 2 lop con:
//   - FoodService    : dich vu an uong, so luong co the tu tinh theo so ban
//                      (10 khach/ban) hoac theo so khach.
//   - PackageService : dich vu tron goi (trang tri, am thanh, MC...), mac dinh 1 goi.
//
// Ham quantityFor() la ham ao thuan (pure virtual). Moi lop con tu quyet dinh
// cach tinh so luong. Nho vay vong lap trong PalaceService chi can goi
// service->quantityFor(...) ma khong can biet do la FoodService hay PackageService.

#include <cstdint>
#include <string>

using namespace std;

class Service {
public:
    Service(string id = "", string code = "", string name = "", string category = "",
            string unit = "", int64_t unitPrice = 0, bool active = true);
    virtual ~Service() = default;

    const string& get_id() const { return id_; }
    const string& get_code() const { return code_; }
    const string& get_name() const { return name_; }
    const string& get_category() const { return category_; }
    const string& get_unit() const { return unit_; }
    int64_t get_unitPrice() const { return unitPrice_; }
    bool is_active() const { return active_; }

    void set_code(const string& value) { code_ = value; }
    void set_name(const string& value) { name_ = value; }
    void set_category(const string& value) { category_ = value; }
    void set_unit(const string& value) { unit_ = value; }
    void set_unitPrice(int64_t value) { unitPrice_ = value; }
    void set_active(bool value) { active_ = value; }

    // Tinh so luong thuc te se dung.
    // enteredQty > 0: nhan vien da nhap tay -> dung luon.
    // enteredQty <= 0: tu dong tinh theo loai dich vu.
    virtual int quantityFor(int guestCount, int enteredQty) const = 0;

    virtual string typeCode() const = 0;  // "FOOD" hoac "PACKAGE"
    virtual string typeName() const = 0;

    string toText() const;

    // Doc mot dong trong data/services.txt va tao doi tuong lop con tuong ung.
    // Nguoi goi phai delete con tro tra ve.
    static Service* fromText(const string& line);

protected:
    string id_;
    string code_;
    string name_;
    string category_;
    string unit_;
    int64_t unitPrice_ = 0;
    bool active_ = true;
};

class FoodService : public Service {
public:
    FoodService(string id = "", string code = "", string name = "", string category = "",
                string unit = "", int64_t unitPrice = 0, bool active = true);

    int quantityFor(int guestCount, int enteredQty) const override;
    string typeCode() const override { return "FOOD"; }
    string typeName() const override { return "Ẩm thực"; }
};

class PackageService : public Service {
public:
    PackageService(string id = "", string code = "", string name = "", string category = "",
                   string unit = "", int64_t unitPrice = 0, bool active = true);

    int quantityFor(int guestCount, int enteredQty) const override;
    string typeCode() const override { return "PACKAGE"; }
    string typeName() const override { return "Trọn gói"; }
};
