#include "DateTime.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// Ham tao co tham so mac dinh: dung chung cho ca DateTime() va DateTime(...).
DateTime::DateTime(int year, int month, int day, int hour, int minute)
    : year_(year), month_(month), day_(day), hour_(hour), minute_(minute) {}

bool DateTime::is_valid() const {
    if (year_ < 2000 || year_ > 2100) return false;
    if (month_ < 1 || month_ > 12) return false;
    if (hour_ < 0 || hour_ > 23 || minute_ < 0 || minute_ > 59) return false;

    static const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month_];

    bool leapYear = (year_ % 4 == 0 && year_ % 100 != 0) || (year_ % 400 == 0);
    if (month_ == 2 && leapYear) maxDay = 29;

    return day_ >= 1 && day_ <= maxDay;
}

int64_t DateTime::toMinutes() const {
    tm parts{};
    parts.tm_year = year_ - 1900;
    parts.tm_mon = month_ - 1;
    parts.tm_mday = day_;
    parts.tm_hour = hour_;
    parts.tm_min = minute_;
    parts.tm_isdst = -1;

#ifdef _WIN32
    time_t seconds = _mkgmtime(&parts);
#else
    time_t seconds = timegm(&parts);
#endif

    if (seconds == static_cast<time_t>(-1)) {
        // Cong thuc du phong neu he thong khong doi duoc.
        return static_cast<int64_t>(year_) * 525600 + month_ * 43200 + day_ * 1440 +
               hour_ * 60 + minute_;
    }
    return static_cast<int64_t>(seconds) / 60;
}

string DateTime::toString() const {
    ostringstream output;
    output << setfill('0') << setw(2) << day_ << '/' << setw(2) << month_ << '/' << year_
           << ' ' << setw(2) << hour_ << ':' << setw(2) << minute_;
    return output.str();
}

ostream& operator<<(ostream& out, const DateTime& value) {
    out << value.toString();
    return out;
}

DateTime DateTime::plusHours(int hours) const {
    time_t seconds = static_cast<time_t>(toMinutes() * 60) + static_cast<time_t>(hours) * 3600;

    tm parts{};
#ifdef _WIN32
    gmtime_s(&parts, &seconds);
#else
    gmtime_r(&seconds, &parts);
#endif

    return DateTime(parts.tm_year + 1900, parts.tm_mon + 1, parts.tm_mday,
                    parts.tm_hour, parts.tm_min);
}

DateTime DateTime::now() {
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());

    tm parts{};
#ifdef _WIN32
    localtime_s(&parts, &currentTime);
#else
    localtime_r(&currentTime, &parts);
#endif

    return DateTime(parts.tm_year + 1900, parts.tm_mon + 1, parts.tm_mday,
                    parts.tm_hour, parts.tm_min);
}

optional<DateTime> DateTime::parse(const string& text) {
    string input = text;
    for (char& c : input) {
        if (c == '-' || c == '.') c = '/';
    }

    int first = 0, second = 0, third = 0;
    char slash1 = 0, slash2 = 0;
    istringstream stream(input);

    if (!(stream >> first >> slash1 >> second >> slash2 >> third)) return nullopt;
    if (slash1 != '/' || slash2 != '/') return nullopt;

    int hour = 0, minute = 0;
    char colon = 0;
    if (stream >> hour) {
        if (stream >> colon >> minute) {
            if (colon != ':') return nullopt;
        } else {
            minute = 0;
        }
    }

    int year = 0, month = 0, day = 0;
    if (first >= 1000) {
        // Dinh dang YYYY/MM/DD.
        year = first;
        month = second;
        day = third;
    } else if (third >= 1000) {
        // Dinh dang DD/MM/YYYY.
        day = first;
        month = second;
        year = third;
    } else {
        return nullopt;
    }

    DateTime result(year, month, day, hour, minute);
    if (!result.is_valid()) return nullopt;
    return result;
}

// Cac toan tu so sanh duoc khai bao friend trong lop nen doc duoc thuoc tinh private.
bool operator<(const DateTime& a, const DateTime& b) {
    if (a.year_ != b.year_) return a.year_ < b.year_;
    if (a.month_ != b.month_) return a.month_ < b.month_;
    if (a.day_ != b.day_) return a.day_ < b.day_;
    if (a.hour_ != b.hour_) return a.hour_ < b.hour_;
    return a.minute_ < b.minute_;
}

bool operator>(const DateTime& a, const DateTime& b) { return b < a; }
bool operator<=(const DateTime& a, const DateTime& b) { return !(b < a); }
bool operator>=(const DateTime& a, const DateTime& b) { return !(a < b); }

bool operator==(const DateTime& a, const DateTime& b) {
    return a.year_ == b.year_ && a.month_ == b.month_ && a.day_ == b.day_ &&
           a.hour_ == b.hour_ && a.minute_ == b.minute_;
}

bool operator!=(const DateTime& a, const DateTime& b) { return !(a == b); }

bool timeOverlaps(const DateTime& aStart, const DateTime& aEnd,
                  const DateTime& bStart, const DateTime& bEnd) {
    return aStart.toMinutes() < bEnd.toMinutes() && bStart.toMinutes() < aEnd.toMinutes();
}

DateTime parseDateOr(const string& text, const DateTime& fallback) {
    auto parsed = DateTime::parse(text);
    return parsed.has_value() ? *parsed : fallback;
}
