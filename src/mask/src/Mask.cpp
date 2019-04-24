#include <Mask.hpp>

#if 0
#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
void (&applyMaskImpl)(const uint8_t *, uint8_t *) = NEON::applyMask;
#else
#include <Mask-Fallback.hpp>
void (&applyMaskImpl)(const uint8_t *, uint8_t *) = Fallback::applyMask;
#endif

std::vector<uint8_t> Mask::generateMask(const uint8_t *imgRGB,
                                        size_t numberOfPixels) {
    std::vector<uint8_t> result;
    result.resize(getPaddedLength(numberOfPixels));
    for (size_t i = 0; i < result.size(); i += 16)
        applyMaskImpl(&imgRGB[3 * i], &result[i]);
    return result;
}
#else

#include "GenerateMask-Unroll.ipp"

#endif
bool Mask::checkImageType(const cv::Mat &img) {
    int dimensions  = img.dims;
    uchar depth     = img.type() & CV_MAT_DEPTH_MASK;
    uchar channels  = 1 + (img.type() >> CV_CN_SHIFT);
    bool contiguous = img.isContinuous();
    return dimensions == 2 && depth == CV_8U && channels == 3 && contiguous;
}

const uint8_t *Mask::toRGB_ptr(const cv::Mat &imgRGB) {
    if (!checkImageType(imgRGB))
        throw std::runtime_error("Error: invalid image format");
    return imgRGB.ptr();
}

Mask::Mask(const uint8_t *imgRGB, uint rows, uint cols)
    : data(generateMask(imgRGB, (size_t) rows * cols)), rows(rows), cols(cols) {
}

Mask::Mask(const cv::Mat &imgRGB)
    : Mask{toRGB_ptr(imgRGB), (uint) imgRGB.rows, (uint) imgRGB.cols} {}

size_t Mask::getPaddedLength(size_t numberOfPixels) {
    return ((15 + numberOfPixels) / 16) * 16;
}