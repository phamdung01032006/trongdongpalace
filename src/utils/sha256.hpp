#pragma once
#include <cstdint>
#include <string>

namespace tdp {

std::string sha256Hex(const std::string& data);
std::string randomSaltHex(int bytes = 8);
std::string hashPassword(const std::string& password, const std::string& saltHex);
bool verifyPassword(const std::string& password, const std::string& saltHex, const std::string& expectedHash);

}  // namespace tdp
