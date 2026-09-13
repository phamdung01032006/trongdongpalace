#pragma once

#include <stdexcept>
#include <string>

using namespace std;

class AppException : public runtime_error {
public:
    explicit AppException(const string& message) : runtime_error(message) {}
};

class ValidationException : public AppException {
public:
    explicit ValidationException(const string& message) : AppException(message) {}
};

class NotFoundException : public AppException {
public:
    explicit NotFoundException(const string& message) : AppException(message) {}
};

class DuplicateException : public AppException {
public:
    explicit DuplicateException(const string& message) : AppException(message) {}
};

class BookingConflictException : public AppException {
public:
    explicit BookingConflictException(const string& message) : AppException(message) {}
};

class AuthenticationException : public AppException {
public:
    explicit AuthenticationException(const string& message) : AppException(message) {}
};

class FileException : public AppException {
public:
    explicit FileException(const string& message) : AppException(message) {}
};
