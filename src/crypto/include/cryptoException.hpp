#pragma once

#include <exception>

class CryptoException : public std::exception {

  public:
    enum ExceptionType { TIMEOUT_EXCEPTION, UNSUCCESSFUL_DECODE };

    ExceptionType exceptionType;
    CryptoException(ExceptionType exceptionType)
        : exceptionType(exceptionType) {}

    ExceptionType getExceptionType() const { return exceptionType; }
};