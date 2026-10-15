#ifndef TDP_MODELS_USER_CPP
#define TDP_MODELS_USER_CPP

#include "models/Enums.cpp"

#include "utils/text_fields.cpp"
#include <string>

using namespace std;

class User {
public:
    User() = default;

    User(const string& id, const string& username, const string& salt, const string& passwordHash,
         const string& fullName, Role role, bool active)
        : id_(id),
          username_(username),
          salt_(salt),
          passwordHash_(passwordHash),
          fullName_(fullName),
          role_(role),
          active_(active) {}

    const string& id() const { return id_; }
    const string& username() const { return username_; }
    const string& salt() const { return salt_; }
    const string& passwordHash() const { return passwordHash_; }
    const string& fullName() const { return fullName_; }
    Role role() const { return role_; }
    bool active() const { return active_; }
    // Tai khoan Customer lien ket 1 khach hang de tu dien form (dac ta Form 1).
    const string& customerId() const { return customerId_; }
    // Mật khẩu dạng rõ chỉ phục vụ demo; đăng nhập vẫn kiểm tra bằng salt + hash.
    const string& demoPassword() const { return demoPassword_; }

    void setFullName(const string& v) { fullName_ = v; }
    void setRole(Role v) { role_ = v; }
    void setActive(bool v) { active_ = v; }
    void setCustomerId(const string& v) { customerId_ = v; }
    void setDemoPassword(const string& v) { demoPassword_ = v; }
    void setPassword(const string& salt, const string& hash) {
        salt_ = salt;
        passwordHash_ = hash;
    }

    vector<string> toFields() const;
    static User fromFields(const vector<string>& f);

private:
    string id_;
    string username_;
    string salt_;
    string passwordHash_;
    string fullName_;
    string customerId_;
    string demoPassword_;  // Du lieu mau de thuan tien demo.
    Role role_ = Role::Customer;
    bool active_ = true;
};

vector<string> User::toFields() const {
    vector<string> f = {id_, username_, salt_, passwordHash_, demoPassword_, fullName_, customerId_, toString(role_), (active_ ? "1" : "0")};
    return f;
}

User User::fromFields(const vector<string>& f) {
    requireFieldCount(f, 9);
    User item;
    item.id_ = f.at(0);
    item.username_ = f.at(1);
    item.salt_ = f.at(2);
    item.passwordHash_ = f.at(3);
    item.demoPassword_ = f.at(4);
    item.fullName_ = f.at(5);
    item.customerId_ = f.at(6);
    item.role_ = roleFromString(f.at(7));
    if (toString(item.role_) != f.at(7)) throw ValidationException("Trang thai/loai khong hop le");
    item.active_ = textBool(f.at(8));
    return item;
}

#endif
