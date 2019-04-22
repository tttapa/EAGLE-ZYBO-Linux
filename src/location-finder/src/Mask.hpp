#pragma once

#include <GridFinder.hpp>
#include <opencv2/core/mat.hpp>

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
#else
#error TODO: fallback mask
#endif

class Mask {
  private:
    const std::vector<uint8_t> data;
    const size_t rows, cols;

  protected:
    static std::vector<uint8_t> applyMask(const uint8_t *imgBGR,
                                          size_t numberOfPixels) {
        std::vector<uint8_t> result;
        result.resize(getPaddedLength(numberOfPixels));
        for (size_t i = 0; i < result.size(); i += 16)
            ::applyMask(&imgBGR[3 * i], &result[i]);
        return result;
    }

    static bool checkType(const cv::Mat &img) {
        uchar depth    = img.type() & CV_MAT_DEPTH_MASK;
        uchar channels = 1 + (img.type() >> CV_CN_SHIFT);
        bool cont      = img.isContinuous();
        return depth == CV_8U && channels == 3 && cont;
    }

    static const uint8_t *toBGRptr(const cv::Mat &imgBGR) {
        if (!checkType(imgBGR))
            throw std::runtime_error("Error: invalid image format");
        return imgBGR.ptr();
    }

  public:
    Mask(const uint8_t *imgBGR, size_t rows, size_t cols)
        : data(applyMask(imgBGR, rows * cols)), rows(rows), cols(cols) {}

    Mask(const cv::Mat &imgBGR)
        : Mask{toBGRptr(imgBGR), imgBGR.rows, imgBGR.cols} {}

    static size_t getPaddedLength(size_t numberOfPixels) {
        return ((15 + numberOfPixels) / 16) * 16;
    }
};