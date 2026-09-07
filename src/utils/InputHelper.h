#pragma once
#include <string>
using namespace std;

class InputHelper {
public:
    static int readInt(const string& prompt);
    static string readString(const string& prompt);
};
