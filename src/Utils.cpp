#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>

using namespace std;

OpResult OpResult::success(const string& message, const string& id) {
    OpResult result;
    result.message = message;
    result.id = id;
    return result;
}

string trim(const string& s) {
    size_t begin = 0;
    while (begin < s.size() && isspace(static_cast<unsigned char>(s[begin]))) ++begin;

    size_t end = s.size();
    while (end > begin && isspace(static_cast<unsigned char>(s[end - 1]))) --end;

    return s.substr(begin, end - begin);
}

string toLower(const string& s) {
    string result = s;
    for (char& c : result) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return result;
}

bool containsIgnoreCase(const string& haystack, const string& needle) {
    return toLower(haystack).find(toLower(needle)) != string::npos;
}

string formatMoney(int64_t amount) {
    bool negative = amount < 0;
    unsigned long long value = negative ? static_cast<unsigned long long>(-amount)
                                        : static_cast<unsigned long long>(amount);

    string digits = to_string(value);
    string grouped;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (count > 0 && count % 3 == 0) grouped.push_back('.');
        grouped.push_back(digits[static_cast<size_t>(i)]);
        ++count;
    }
    reverse(grouped.begin(), grouped.end());
    return (negative ? "-" : "") + grouped + " đ";
}

optional<int64_t> parseMoney(const string& s) {
    string digits;
    for (char c : s) {
        if (isdigit(static_cast<unsigned char>(c)) || c == '-') digits.push_back(c);
    }
    if (digits.empty() || digits == "-") return nullopt;

    try {
        size_t index = 0;
        long long value = stoll(digits, &index);
        if (index != digits.size()) return nullopt;
        return static_cast<int64_t>(value);
    } catch (const invalid_argument&) {
        return nullopt;
    } catch (const out_of_range&) {
        return nullopt;
    }
}

optional<int> parseInt(const string& s) {
    string text = trim(s);
    if (text.empty()) return nullopt;

    try {
        size_t index = 0;
        int value = stoi(text, &index);
        if (index != text.size()) return nullopt;
        return value;
    } catch (const invalid_argument&) {
        return nullopt;
    } catch (const out_of_range&) {
        return nullopt;
    }
}

string nextId(const string& prefix, const vector<string>& existingIds) {
    int maxNumber = 0;
    for (const string& id : existingIds) {
        if (id.rfind(prefix, 0) != 0) continue;
        try {
            int number = stoi(id.substr(prefix.size()));
            if (number > maxNumber) maxNumber = number;
        } catch (const invalid_argument&) {
            // Bo qua cac ma khong dung dinh dang.
        } catch (const out_of_range&) {
            // Bo qua cac ma qua lon.
        }
    }

    ostringstream output;
    output << prefix << setfill('0') << setw(4) << (maxNumber + 1);
    return output.str();
}

bool looksLikePhone(const string& phone) {
    string digits;
    for (char c : phone) {
        if (c == ' ' || c == '-' || c == '.') continue;
        digits.push_back(c);
    }
    if (digits.size() < 9 || digits.size() > 12) return false;
    for (char c : digits) {
        if (!isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

bool looksLikeEmail(const string& email) {
    if (email.empty()) return true;
    static const regex pattern(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    return regex_match(email, pattern);
}

string cleanField(const string& s) {
    string result = s;
    for (char& c : result) {
        if (c == '|' || c == '\n' || c == '\r') c = ' ';
    }
    return trim(result);
}

vector<string> splitText(const string& line, char delimiter) {
    vector<string> parts;
    string current;
    for (char c : line) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    parts.push_back(current);
    return parts;
}

string joinText(const vector<string>& parts, char delimiter) {
    string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result.push_back(delimiter);
        result += parts[i];
    }
    return result;
}

string fieldAt(const vector<string>& parts, size_t index, const string& defaultValue) {
    if (index >= parts.size()) return defaultValue;
    return parts[index];
}

int toInt(const string& s, int defaultValue) {
    optional<int> value = parseInt(s);
    return value.has_value() ? *value : defaultValue;
}

int64_t toInt64(const string& s, int64_t defaultValue) {
    optional<int64_t> value = parseMoney(s);
    return value.has_value() ? *value : defaultValue;
}

bool toBool(const string& s, bool defaultValue) {
    string text = toLower(trim(s));
    if (text == "1" || text == "true" || text == "co" || text == "có") return true;
    if (text == "0" || text == "false" || text == "khong" || text == "không") return false;
    return defaultValue;
}

string boolText(bool value) { return value ? "1" : "0"; }
