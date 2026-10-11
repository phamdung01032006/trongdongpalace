#include "utils/helpers.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <regex>
#include <sstream>

namespace tdp {

OpResult OpResult::success(const std::string& message, const std::string& id) {
    return OpResult{true, message, id};
}
OpResult OpResult::fail(const std::string& message) { return OpResult{false, message, ""}; }

std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}

bool DateTime::valid() const {
    if (year < 2000 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return false;
    static const int mdays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int dim = mdays[month];
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leap) dim = 29;
    return day >= 1 && day <= dim;
}

int64_t DateTime::toMinutes() const {
    std::tm tm{};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_isdst = -1;
#ifdef _WIN32
    time_t t = _mkgmtime(&tm);
#else
    time_t t = timegm(&tm);
#endif
    if (t == static_cast<time_t>(-1)) {
        return static_cast<int64_t>(year) * 525600 + month * 43200 + day * 1440 + hour * 60 + minute;
    }
    return static_cast<int64_t>(t) / 60;
}

std::string DateTime::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << '/' << std::setw(2) << month << '/' << year << ' '
        << std::setw(2) << hour << ':' << std::setw(2) << minute;
    return oss.str();
}

DateTime DateTime::now() {
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    DateTime d;
    d.year = tm.tm_year + 1900;
    d.month = tm.tm_mon + 1;
    d.day = tm.tm_mday;
    d.hour = tm.tm_hour;
    d.minute = tm.tm_min;
    return d;
}

std::optional<DateTime> DateTime::parse(const std::string& text) {
    std::string s = trim(text);
    if (s.empty()) return std::nullopt;
    int p1 = 0, p2 = 0, p3 = 0, h = 0, mi = 0;
    char c1 = 0, c2 = 0, c3 = 0;
    std::istringstream iss(s);
    if (!(iss >> p1 >> c1 >> p2 >> c2 >> p3)) return std::nullopt;
    if ((c1 != '/' && c1 != '-') || (c2 != '/' && c2 != '-')) return std::nullopt;
    if (iss >> h) {
        if (iss >> c3 >> mi) {
            if (c3 != ':') return std::nullopt;
        } else {
            mi = 0;
        }
    } else {
        h = 0;
        mi = 0;
    }
    int y = 0, m = 0, d = 0;
    if (p1 >= 1000) {
        // YYYY-MM-DD or YYYY/MM/DD
        y = p1;
        m = p2;
        d = p3;
    } else if (p3 >= 1000) {
        // DD/MM/YYYY or DD-MM-YYYY
        d = p1;
        m = p2;
        y = p3;
    } else {
        return std::nullopt;
    }
    DateTime dt{y, m, d, h, mi};
    if (!dt.valid()) return std::nullopt;
    return dt;
}

bool operator<(const DateTime& a, const DateTime& b) { return a.toMinutes() < b.toMinutes(); }
bool operator<=(const DateTime& a, const DateTime& b) { return a.toMinutes() <= b.toMinutes(); }
bool operator==(const DateTime& a, const DateTime& b) { return a.toMinutes() == b.toMinutes(); }

bool timeOverlaps(const DateTime& aStart, const DateTime& aEnd, const DateTime& bStart, const DateTime& bEnd) {
    return aStart.toMinutes() < bEnd.toMinutes() && bStart.toMinutes() < aEnd.toMinutes();
}

std::string formatMoney(int64_t amount) {
    bool neg = amount < 0;
    uint64_t v = static_cast<uint64_t>(neg ? -amount : amount);
    std::string digits = std::to_string(v);
    std::string grouped;
    int cnt = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (cnt && cnt % 3 == 0) grouped.push_back('.');
        grouped.push_back(digits[static_cast<size_t>(i)]);
        ++cnt;
    }
    std::reverse(grouped.begin(), grouped.end());
    return (neg ? "-" : "") + grouped + " đ";
}

std::optional<int64_t> parseMoney(const std::string& s) {
    std::string t;
    for (char c : trim(s)) {
        if (c == '.' || c == ',' || c == ' ') continue;
        t.push_back(c);
    }
    if (t.empty()) return std::nullopt;
    try {
        size_t idx = 0;
        long long v = std::stoll(t, &idx, 10);
        if (idx != t.size()) return std::nullopt;
        return static_cast<int64_t>(v);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<int> parseInt(const std::string& s) {
    std::string t = trim(s);
    if (t.empty()) return std::nullopt;
    try {
        size_t idx = 0;
        int v = std::stoi(t, &idx, 10);
        if (idx != t.size()) return std::nullopt;
        return v;
    } catch (...) {
        return std::nullopt;
    }
}

std::string nextId(const std::string& prefix, const std::vector<std::string>& existingIds) {
    int maxNo = 0;
    for (const auto& id : existingIds) {
        if (id.rfind(prefix, 0) != 0) continue;
        try {
            int n = std::stoi(id.substr(prefix.size()));
            if (n > maxNo) maxNo = n;
        } catch (...) {
        }
    }
    std::ostringstream oss;
    oss << prefix << std::setfill('0') << std::setw(4) << (maxNo + 1);
    return oss.str();
}

bool looksLikePhone(const std::string& phone) {
    std::string t;
    for (char c : phone) {
        if (c == ' ' || c == '-' || c == '.') continue;
        t.push_back(c);
    }
    if (t.size() < 9 || t.size() > 12) return false;
    for (char c : t)
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

bool looksLikeEmail(const std::string& email) {
    if (email.empty()) return true;
    static const std::regex re(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    return std::regex_match(email, re);
}

int discountPercentForSubtotal(int64_t subtotal) {
    // Giả định thiết kế (PDF không có trong repo): chiết khấu theo tổng trước CK.
    if (subtotal >= 100000000) return 8;
    if (subtotal >= 50000000) return 5;
    if (subtotal >= 20000000) return 3;
    return 0;
}

}  // namespace tdp
