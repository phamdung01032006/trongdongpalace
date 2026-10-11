#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace tdp {

struct DateTime {
    int year = 1970;
    int month = 1;
    int day = 1;
    int hour = 0;
    int minute = 0;

    static DateTime now();
    static std::optional<DateTime> parse(const std::string& text);
    std::string toString() const;
    int64_t toMinutes() const;
    bool valid() const;
};

bool operator<(const DateTime& a, const DateTime& b);
bool operator<=(const DateTime& a, const DateTime& b);
bool operator==(const DateTime& a, const DateTime& b);
bool timeOverlaps(const DateTime& aStart, const DateTime& aEnd, const DateTime& bStart, const DateTime& bEnd);

struct OpResult {
    bool ok = false;
    std::string message;
    std::string id;

    static OpResult success(const std::string& message = "Thành công", const std::string& id = "");
    static OpResult fail(const std::string& message);
};

std::string trim(const std::string& s);
std::string toLower(const std::string& s);
bool containsIgnoreCase(const std::string& haystack, const std::string& needle);
std::string formatMoney(int64_t amount);
std::optional<int64_t> parseMoney(const std::string& s);
std::optional<int> parseInt(const std::string& s);
std::string nextId(const std::string& prefix, const std::vector<std::string>& existingIds);
bool looksLikePhone(const std::string& phone);
bool looksLikeEmail(const std::string& email);

int discountPercentForSubtotal(int64_t subtotal);

}  // namespace tdp
