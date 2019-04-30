#pragma once

int test();

#include <AngleTracker.hpp>
#include <HelperStructs.hpp>
#include <opencv2/videoio.hpp>

class LocationFinder {
  public:
    LocationFinder(cv::VideoCapture &&cap) : cap(cap) {}
    LocationFinder(const cv::VideoCapture &cap) : cap(cap) {}
    LocationFinder(int i) : cap(i) {}
    LocationFinder(std::string s) : cap(s) {}

    using Vec2f = TColVector<float, 2>;

    /**
     * @brief   Read a new frame, mask it, run GridFinder, and get the position
     *          of the drone. The position is the position within the square, 
     *          between 0.0 and 1.0.
     * 
     * @return  The position within the square.
     */
    Point updateLocation();
    /**
     * @brief   Get the position of the center of the frame relative to the
     *          given square. 
     * 
     * @param   sq
     *          A square of the reference grid.
     * @param   frameCenter 
     *          The center of the frame, i.e. the point that will be projected
     *          onto the sides of the square.
     * @return  The position of the frame center within the square, modulo 1.0.
     */
    Point getLocation(Square &sq, Vec2f frameCenter);

    angle_t getAngle() const { return angle; }
    cv::Mat getImage() const { return image; }
    cv::Mat getMaskImage() const { return maskImage; }
    cv::VideoCapture &getCapture() { return cap; }

  private:
    cv::VideoCapture cap;
    cv::Mat image;
    cv::Mat maskImage;
    bool hasImage = false;
    AngleTracker angleTracker;
    float sideLen = 0.0;
    angle_t angle;
};