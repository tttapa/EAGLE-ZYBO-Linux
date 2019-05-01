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

    const char *what() const noexcept override {
        switch (getExceptionType()) {
            case TIMEOUT_EXCEPTION: return "TIMEOUT_EXCEPTION";
            case UNSUCCESSFUL_DECODE_EXCEPTION:
                return "UNSUCCESSFUL_DECODE_EXCEPTION";
            case UNKNOWN_ERROR_EXCEPTION: return "UNKNOWN_ERROR_EXCEPTION";
            default:
                return "Error: invalid enum state for "
                       "CryptoException::ExceptionType";
        }
    }
};