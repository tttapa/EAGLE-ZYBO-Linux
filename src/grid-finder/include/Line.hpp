#pragma once

#include <Angle.hpp>
#include <Matrix.hpp>
#include <Pixel.hpp>
#include <ostream>

#include <iostream>  // TODO

struct Point;
inline std::ostream &operator<<(std::ostream &os, Point p);

struct Point {
    float x;
    float y;

    constexpr bool operator==(Point rhs) const {
        return this->x == rhs.x && this->y == rhs.y;
    }

    operator TColVector<float, 2>() const { return vec(); }
    TColVector<float, 2> vec() const { return {x, y}; }

    constexpr Point() : x{nan("")}, y{nan("")} {}
    constexpr Point(float x, float y) : x{x}, y{y} {}
    constexpr Point(Pixel px) : x(px.x), y(px.y) {}
    constexpr Point(const TColVector<float, 2> &vec) : x{vec[0]}, y{vec[1]} {}

    constexpr explicit operator bool() const {
        return std::isfinite(x) && std::isfinite(y);
    }

    constexpr Point operator%(float f) const {
        return {
            std::fmod(x, f),
            std::fmod(y, f),
        };
    }

    static Point invalid() { return {}; }

    static float distance(Point a, Point b) {
        auto result = sqrtf(distanceSquared(a, b));
        // std::cout << "sqrtf(distanceSquared(a, b)) = " << result << std::endl;
        return result;
    }
    static float distanceSquared(Point a, Point b) {
        using namespace std;
        // cout << __PRETTY_FUNCTION__ << endl;
        // cout << "a = " << a << ", b = " << b << endl;
        auto diff = a.vec() - b.vec();
        // cout << "diff = " << Point(diff) << endl;
        auto result = normsq(diff);
        // cout << "normsq(diff) = " << result << endl;
        return result;
    }
    static Point average(Point a, Point b) { return (a.vec() + b.vec()) / 2.0; }
};

inline std::ostream &operator<<(std::ostream &os, Point p) {
    return os << '(' << p.x << ", " << p.y << ')';
}

template <uint R>
constexpr Point rotate(Point p, Angle<R> a) {
    return {
        p.x * a.cosf() - p.y * a.sinf(),
        p.x * a.sinf() + p.y * a.cosf(),
    };
}

constexpr inline Point round(Point p) {
    return {
        std::round(p.x),
        std::round(p.y),
    };
}

class Line {
    using ColVec3f = TColVector<float, 3>;

  public:
    Line(ColVec3f homogCoordinates) : homogCoordinates(homogCoordinates) {}
    Line(Pixel p, CosSin slope)
        : homogCoordinates{
              slope.sinf(),
              -slope.cosf(),
              -slope.sinf() * p.x + slope.cosf() * p.y,
          } {}

    bool rightOfPoint(Pixel point) {
        float signedDistance = homogCoordinates[0] * point.x +
                               homogCoordinates[1] * point.y +
                               homogCoordinates[2];
        return signedDistance >= 0;
    }

    bool leftOfPoint(Pixel point) {
        float signedDistance = homogCoordinates[0] * point.x +
                               homogCoordinates[1] * point.y +
                               homogCoordinates[2];
        return signedDistance <= 0;
    }

    static Point intersect(Line a, Line b) {
        ColVec3f crss = cross(a.homogCoordinates, b.homogCoordinates);
        return {crss[0] / crss[2], crss[1] / crss[2]};
    }

  private:
    const ColVec3f homogCoordinates;
};