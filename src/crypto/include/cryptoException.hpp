#pragma once

#include <exception>

class CryptoException : public std::exception {
  public:
    enum ExceptionType {
        TIMEOUT_EXCEPTION,
        UNSUCCESSFUL_DECODE_EXCEPTION,
        UNKNOWN_ERROR_EXCEPTION
    };

  private:
    ExceptionType exceptionType;

  public:
    CryptoException(ExceptionType exceptionType)
        : exceptionType(exceptionType) {}

    ExceptionType getExceptionType() const { return exceptionType; }
};