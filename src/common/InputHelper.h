#pragma once

#include <optional>
#include <string>

using namespace std;

class InputHelper {
public:
    static string readString(const string& prompt, bool allowEmpty = false);
    static int readInt(const string& prompt, optional<int> minimum = nullopt,
                       optional<int> maximum = nullopt);
    static double readDouble(const string& prompt, optional<double> minimum = nullopt,
                             optional<double> maximum = nullopt);
    static bool readYesNo(const string& prompt);
};
