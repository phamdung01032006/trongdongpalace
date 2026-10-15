#ifndef TDP_TEXT_FIELDS_CPP
#define TDP_TEXT_FIELDS_CPP

#include "utils/helpers.cpp"
#include "models/Exceptions.cpp"
#include <limits>
#include <stdexcept>
#include <sstream>

using namespace std;

// Moi ban ghi la mot dong. Escape giup ten/ghi chu chua |, \ va xuong dong.
string joinTextFields(const vector<string>& fields) {
    string out;
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) out += '|';
        for (char c : fields[i]) {
            if (c == '\\') out += "\\\\";
            else if (c == '|') out += "\\|";
            else if (c == '\n') out += "\\n";
            else if (c == '\r') out += "\\r";
            else out += c;
        }
    }
    return out;
}

vector<string> splitTextFields(const string& line) {
    vector<string> fields;
    string field;
    bool escaped = false;
    for (char c : line) {
        if (escaped) {
            if (c == 'n') field += '\n';
            else if (c == 'r') field += '\r';
            else if (c == '\\' || c == '|') field += c;
            else throw ValidationException("Ky tu escape khong hop le");
            escaped = false;
        } else if (c == '\\') escaped = true;
        else if (c == '|') { fields.push_back(field); field.clear(); }
        else field += c;
    }
    if (escaped) throw ValidationException("Thieu ky tu sau dau escape");
    fields.push_back(field);
    return fields;
}

void requireFieldCount(const vector<string>& f, size_t count) {
    if (f.size() != count) throw ValidationException("Sai so cot du lieu");
}

long long textMoney(const string& s) {
    size_t used = 0;
    long long value;
    try {
        value = stoll(s, &used);
    } catch (const invalid_argument&) {
        throw ValidationException("So khong hop le: " + s);
    } catch (const out_of_range&) {
        throw ValidationException("So vuot gioi han long long: " + s);
    }
    if (used != s.size() || value < 0) throw ValidationException("So khong hop le: " + s);
    return value;
}

int textInt(const string& s) {
    auto value = textMoney(s);
    if (value > numeric_limits<int>::max()) throw ValidationException("So vuot gioi han int");
    return static_cast<int>(value);
}

bool textBool(const string& s) {
    if (s == "1") return true;
    if (s == "0") return false;
    throw ValidationException("Gia tri bool phai la 0 hoac 1");
}

DateTime textDate(const string& s) {
    auto value = DateTime::parse(s);
    if (!value) throw ValidationException("Ngay gio khong hop le: " + s);
    return *value;
}

#endif
