#pragma once

// Lop DateTime tu viet de khong phu thuoc thu vien ngoai.
// Cac thuoc tinh duoc dong goi (private), truy cap qua get_.../set_...
// Cac phep so sanh <, <=, == ... duoc tai boi (overload) va khai bao friend
// vi chung can doc truc tiep nam/thang/ngay/gio/phut.

#include <cstdint>
#include <optional>
#include <ostream>
#include <string>

using namespace std;

class DateTime {
public:
    // Ham tao co tham so mac dinh: DateTime() chinh la 01/01/1970 00:00.
    DateTime(int year = 1970, int month = 1, int day = 1, int hour = 0, int minute = 0);

    int get_year() const { return year_; }
    int get_month() const { return month_; }
    int get_day() const { return day_; }
    int get_hour() const { return hour_; }
    int get_minute() const { return minute_; }

    void set_year(int value) { year_ = value; }
    void set_month(int value) { month_ = value; }
    void set_day(int value) { day_ = value; }
    void set_hour(int value) { hour_ = value; }
    void set_minute(int value) { minute_ = value; }

    static DateTime now();

    // Nhan cac dinh dang: "25/12/2026 18:30", "25-12-2026 18:30",
    // "2026-12-25 18:30". Neu sai dinh dang tra ve nullopt.
    static optional<DateTime> parse(const string& text);

    string toString() const;  // "25/12/2026 18:30"
    DateTime plusHours(int hours) const;
    int64_t toMinutes() const;
    bool is_valid() const;

    // Cac toan tu so sanh doi xung: dung friend de truy cap truc tiep thuoc tinh.
    friend bool operator<(const DateTime& a, const DateTime& b);
    friend bool operator>(const DateTime& a, const DateTime& b);
    friend bool operator<=(const DateTime& a, const DateTime& b);
    friend bool operator>=(const DateTime& a, const DateTime& b);
    friend bool operator==(const DateTime& a, const DateTime& b);
    friend bool operator!=(const DateTime& a, const DateTime& b);

    // Toan tu xuat << la ham friend de in DateTime truc tiep ra man hinh.
    friend ostream& operator<<(ostream& out, const DateTime& value);

private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
};

// Hai khoang thoi gian [aStart, aEnd) va [bStart, bEnd) co giao nhau khong.
bool timeOverlaps(const DateTime& aStart, const DateTime& aEnd,
                  const DateTime& bStart, const DateTime& bEnd);

// Tien ich khi doc file: neu chuoi sai dinh dang thi tra ve gia tri mac dinh.
DateTime parseDateOr(const string& text, const DateTime& fallback = DateTime{});
