#pragma once

#include <Angle.hpp>
#include <Degrees.hpp>
#include <Line.hpp>
#include <Pixel.hpp>
#include <optional>

#pragma region Structs..........................................................

/**
 * @brief   Struct as a result of the hough function. It gives the most 
 *          plausible angle, and a vote count that's an indication of how many
 *          white pixels lie on that line (may be weighted and truncated).
 * @see     GridFinder::hough
 */
struct HoughResult {
    angle_t angle;
    uint count;
    bool operator<(HoughResult other) const { return count < other.count; }
};

struct LineResult {
    Pixel lineCenter;
    uint width;
    angle_t angle;
};

struct FirstLineEstimate {
    Pixel middle;
    uint width;
    angle_t angleEstimate;
};

struct GetMiddleResult {
    Pixel pixel;
    uint width;
};

struct Square {
    std::array<std::optional<LineResult>, 5> lines;
    std::array<std::optional<Point>, 4> points;

    /**
     * @brief   Get the average angle of the sides of the square 
     *          @f$ \in [0°, 90°) @f$
     * 
     * Assumes all four sides are present, and they are more or less 
     * perpendicular.
     */
    angle_t getAngle() const {
        if (!lines[0] || !lines[1] || !lines[2] || !lines[3] || !lines[4])
            throw std::runtime_error(
                "Error: cannot determine angle: not all lines present");

        const int _45  = angle_t(45_deg).getIndex();
        const int _90  = angle_t(90_deg).getIndex();
        const int _360 = angle_t::resolution();

        uint first_angle =
            angle_t::average(lines[0]->angle, lines[1]->angle.opposite())
                .getIndex();
        std::array<uint, 4> angles = {
            first_angle,
            lines[2]->angle.getIndex(),
            lines[3]->angle.getIndex(),
            lines[4]->angle.getIndex(),
        };
        uint sum = 0;
        for (uint angle : angles) {
            uint v = ((angle + _45 + _360 - first_angle) % _90) +
                     (first_angle % _90)  //
                     + _360               // make sure that the result is > 0
                     - _45;
            std::cout << v << "; ";
            sum += v;
        }
        std::cout << std::endl;
        return angle_t{(sum / 4) % _90};
    }
};

#pragma endregion
#pragma region Printing.........................................................

/**
 * @brief   Function for printing a HoughResult.
 * @see     HoughResult
 */
inline std::ostream &operator<<(std::ostream &os, HoughResult h) {
    return os << h.angle << " rad: " << h.count;
}

inline std::ostream &operator<<(std::ostream &os, LineResult l) {
    return os << "LineResult(" << l.lineCenter << ", " << l.width << ", "
              << l.angle << ")";
}

inline std::ostream &operator<<(std::ostream &os, const Square &sq) {
    os << "Square [ ";
    for (const std::optional<Point> &p : sq.points) {
        if (p)
            os << *p << ' ';
        else
            os << "ϕ ";
    }
    return os << ']';
}

#pragma endregion