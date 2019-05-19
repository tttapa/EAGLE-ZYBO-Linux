#pragma once

#include <EulerAngles.hpp>

// TODO: Documentation

enum class AngleFormat {
    radians,
    degrees,
    degreesTeX,
};

struct PrintAsEulerAngles : public Printable {
    PrintAsEulerAngles(const EulerAngles &e, AngleFormat format,
                       size_t precision)
        : e{e}, format{format}, precision{precision} {}

    void print(std::ostream &os) const override {
        os << '(';                            //
        formatAngle(os, e.roll) << " X\", ";  //
        formatAngle(os, e.pitch) << " Y', ";  //
        formatAngle(os, e.yaw) << " Z)";      //
    }

  private:
    const EulerAngles e;
    AngleFormat format;
    const size_t precision;

    std::ostream &formatAngle(std::ostream &os, double d) const {
        const char *unit;
        switch (format) {
            case AngleFormat::degrees:
            case AngleFormat::degreesTeX: d *= 180.0 / M_PI; break;
            case AngleFormat::radians:
            default:;
        }
        switch (format) {
            case AngleFormat::degrees: unit = "°"; break;
            case AngleFormat::degreesTeX: unit = "\\degree"; break;
            case AngleFormat::radians:
            default: unit = " rad";
        }
        return os << std::setprecision(precision) << std::setw(precision + 6)
                  << d << unit;
    }
};

inline PrintAsEulerAngles
asEulerAngles(const EulerAngles &e, AngleFormat format = AngleFormat::radians,
              size_t precision = 2) {
    return {e, format, precision};
}

inline PrintAsEulerAngles asEulerAngles(const EulerAngles &e,
                                        size_t precision) {
    return {e, AngleFormat::radians, precision};
}