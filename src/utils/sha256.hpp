#pragma once
#include <cstdint>
#include <string>

using namespace std;

namespace tdp {

string sha256Hex(const string& data);
string randomSaltHex(int bytes = 8);
string hashPassword(const string& password, const string& saltHex);
bool verifyPassword(const string& password, const string& saltHex, const string& expectedHash);

}  // namespace tdp
