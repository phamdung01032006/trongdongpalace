#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

using namespace std;

namespace tdp {

struct DateTime {
    int year = 1970;
    int month = 1;
    int day = 1;
    int hour = 0;
    int minute = 0;

    static DateTime now();
    static optional<DateTime> parse(const string& text);
    string toString() const;
    int64_t toMinutes() const;
    bool valid() const;
};

bool operator<(const DateTime& a, const DateTime& b);
bool operator<=(const DateTime& a, const DateTime& b);
bool operator==(const DateTime& a, const DateTime& b);
bool timeOverlaps(const DateTime& aStart, const DateTime& aEnd, const DateTime& bStart, const DateTime& bEnd);

struct OpResult {
    bool ok = false;
    string message;
    string id;

    static OpResult success(const string& message = "Thành công", const string& id = "");
    static OpResult fail(const string& message);
};

string trim(const string& s);
string toLower(const string& s);
bool containsIgnoreCase(const string& haystack, const string& needle);
string formatMoney(int64_t amount);
optional<int64_t> parseMoney(const string& s);
optional<int> parseInt(const string& s);
string nextId(const string& prefix, const vector<string>& existingIds);
bool looksLikePhone(const string& phone);
bool looksLikeEmail(const string& email);

}  // namespace tdp
