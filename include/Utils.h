#pragma once

// Cac ham tien ich dung chung cho ca du an.
// Sinh vien chi can hieu: day la cac ham "lam viec nho" nhu cat khoang trang,
// doi chuoi thanh so, dinh dang tien... de cac file khac khong bi lap code.

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// ==================== HAM MAU (TEMPLATE) ====================
// Nhieu lop (User, Customer, Hall, EventRequest, Booking, Payment...) deu co
// get_id(). Ham mau nay dung chung logic tim theo id, tranh lap code cho tung
// kieu du lieu cu the.
template <typename T>
T* findById(vector<T>& items, const string& id) {
    for (T& item : items) {
        if (item.get_id() == id) return &item;
    }
    return nullptr;
}

template <typename T>
const T* findById(const vector<T>& items, const string& id) {
    for (const T& item : items) {
        if (item.get_id() == id) return &item;
    }
    return nullptr;
}

// ==================== NGOAI LE NGHIEP VU ====================
// Lop co so cho cac loi cua chuong trinh. Nho ke thua std::runtime_error,
// main() co the bat bang catch (const exception&).
class AppException : public runtime_error {
public:
    explicit AppException(const string& message) : runtime_error(message) {}
};

// Loi nghiep vu: du lieu nhap sai, khong du quyen, trung lich...
// Service nem loi nay; giao dien bat va in thong bao cho nguoi dung.
class BusinessException : public AppException {
public:
    explicit BusinessException(const string& message) : AppException(message) {}
};

// Loi thao tac file: khong mo duoc file, ghi file that bai...
class FileException : public AppException {
public:
    explicit FileException(const string& message) : AppException(message) {}
};

// Ket qua tra ve khi thao tac THANH CONG.
// Loi khong con duoc tra ve bang OpResult ma nem BusinessException.
struct OpResult {
    string message;
    string id;  // Ma cua doi tuong vua tao/sua (neu co).

    static OpResult success(const string& message = "Thanh cong", const string& id = "");
};

string trim(const string& s);
string toLower(const string& s);
bool containsIgnoreCase(const string& haystack, const string& needle);

// Dinh dang tien kieu Viet Nam: 12000000 -> "12.000.000 d".
string formatMoney(int64_t amount);
optional<int64_t> parseMoney(const string& s);
optional<int> parseInt(const string& s);

// Sinh ma tu dong: nextId("REQ-", {"REQ-0001"}) -> "REQ-0002".
string nextId(const string& prefix, const vector<string>& existingIds);

bool looksLikePhone(const string& phone);
bool looksLikeEmail(const string& email);

// --- Xu ly file text ---
// Moi dong trong file data/*.txt la mot ban ghi, cac truong cach nhau boi '|'.
// cleanField thay '|' va xuong dong bang khoang trang de file khong bi vo.
string cleanField(const string& s);
vector<string> splitText(const string& line, char delimiter);
string joinText(const vector<string>& parts, char delimiter);
string fieldAt(const vector<string>& parts, size_t index, const string& defaultValue = "");

int toInt(const string& s, int defaultValue = 0);
int64_t toInt64(const string& s, int64_t defaultValue = 0);
bool toBool(const string& s, bool defaultValue = false);
string boolText(bool value);
