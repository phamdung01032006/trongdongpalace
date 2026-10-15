#ifndef TDP_MODELS_EXCEPTIONS_CPP
#define TDP_MODELS_EXCEPTIONS_CPP

#include <stdexcept>
#include <string>

using namespace std;

class AppException : public runtime_error {
public:
    explicit AppException(const string& message) : runtime_error(message) {}
};

class ValidationException : public AppException {
public:
    using AppException::AppException;
};
class NotFoundException : public AppException {
public:
    using AppException::AppException;
};
class DuplicateException : public AppException {
public:
    using AppException::AppException;
};
class BookingConflictException : public AppException {
public:
    using AppException::AppException;
};
class AuthenticationException : public AppException {
public:
    using AppException::AppException;
};
class FileException : public AppException {
public:
    using AppException::AppException;
};

#endif
