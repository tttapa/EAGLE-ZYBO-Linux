#pragma once

#include <exception>
#include <string>

class CryptoException : public std::exception {

  public:
    enum class ExceptionType {
        TIMEOUT_EXCEPTION,
        UNSUCCESSFUL_DECODE_EXCEPTION,
        UNKNOWN_ERROR_EXCEPTION
    };

  private:
    ExceptionType exceptionType;
    std::string message;

  public:
    CryptoException(ExceptionType exceptionType, std::string message = "")
        : exceptionType(exceptionType), message(message) {}

    ExceptionType getExceptionType() const { return exceptionType; }

    const char *what() const noexcept override {
        switch (getExceptionType()) {
            case ExceptionType::TIMEOUT_EXCEPTION: return "TIMEOUT_EXCEPTION";
            case ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION:
                return "UNSUCCESSFUL_DECODE_EXCEPTION";
            case ExceptionType::UNKNOWN_ERROR_EXCEPTION:
                return ("UNKNOWN_ERROR_EXCEPTION: " + message).c_str();
            default:
                return "Error: invalid enum state for "
                       "CryptoException::ExceptionType";
        }
    }
};