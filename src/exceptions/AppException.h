#pragma once

#include <exception>
#include <string>

using namespace std;

class AppException : public exception {
private:
    string message;

public:
    explicit AppException(const string& message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class ValidationException : public AppException {
public:
    explicit ValidationException(const string& message) : AppException(message) {}
};

class EntityNotFoundException : public AppException {
public:
    explicit EntityNotFoundException(const string& message) : AppException(message) {}
};

class DatabaseException : public AppException {
public:
    explicit DatabaseException(const string& message) : AppException(message) {}
};

class BusinessException : public AppException {
public:
    explicit BusinessException(const string& message) : AppException(message) {}
};

class BookingConflictException : public BusinessException {
public:
    explicit BookingConflictException(const string& message) : BusinessException(message) {}
};

class PaymentException : public BusinessException {
public:
    explicit PaymentException(const string& message) : BusinessException(message) {}
};
