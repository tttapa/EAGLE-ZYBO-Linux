#include <Mask.hpp>

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
#else
#error TODO: fallback mask
#endif

std::vector<uint8_t> Mask::applyMask(const uint8_t *imgBGR,
                                     size_t numberOfPixels) {
    std::vector<uint8_t> result;
    result.resize(getPaddedLength(numberOfPixels));
    for (size_t i = 0; i < result.size(); i += 16)
        ::applyMask(&imgBGR[3 * i], &result[i]);
    return result;
}

bool Mask::checkImageType(const cv::Mat &img) {
    uchar depth     = img.type() & CV_MAT_DEPTH_MASK;
    uchar channels  = 1 + (img.type() >> CV_CN_SHIFT);
    bool contiguous = img.isContinuous();
    return depth == CV_8U && channels == 3 && contiguous;
}

const uint8_t *Mask::toBGR_ptr(const cv::Mat &imgBGR) {
    if (!checkImageType(imgBGR))
        throw std::runtime_error("Error: invalid image format");
    return imgBGR.ptr();
}

Mask::Mask(const uint8_t *imgBGR, size_t rows, size_t cols)
    : data(applyMask(imgBGR, rows * cols)), rows(rows), cols(cols) {}

Mask::Mask(const cv::Mat &imgBGR)
    : Mask{toBGR_ptr(imgBGR), imgBGR.rows, imgBGR.cols} {}

size_t Mask::getPaddedLength(size_t numberOfPixels) {
    return ((15 + numberOfPixels) / 16) * 16;
}