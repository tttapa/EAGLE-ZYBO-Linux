#pragma once

#include <GridFinder.hpp>
#include <opencv2/core/mat.hpp>

class Mask {
  public:
    /**
     * @brief   Construct a new Mask object from a plain array of interleaved 
     *          BGR bytes.
     * 
     * @param   imgBGR
     *          A pointer to an array of interleaved BGR bytes.
     * @param   rows 
     *          The height of the image, or the number of rows in the pixel 
     *          matrix.
     * @param   cols 
     *          The width of the image, or the number of columns in the pixel
     *          matrix.
     */
    Mask(const uint8_t *imgBGR, size_t rows, size_t cols);
    /**
     * @brief   Construct a new Mask object from an OpenCV image.
     * 
     * @param   imgBGR 
     *          An OpenCV image.
     */
    Mask(const cv::Mat &imgBGR);

  protected:
    /**
     * @brief   Get the padded length for the data vector.
     * 
     * Because the implementation of the masking uses vector instructions that
     * handle 16 pixels at a time, we always allocate more memory than needed
     * to make sure we never write out of bounds.
     * 
     * @param   numberOfPixels
     *          The actual number of pixels in the original image.
     * @return  The size in bytes of mask data, rounded up to the nearest 
     *          multiple of 16.
     */
    static size_t getPaddedLength(size_t numberOfPixels);
    static std::vector<uint8_t> applyMask(const uint8_t *imgBGR,
                                          size_t numberOfPixels);
    static bool checkImageType(const cv::Mat &img);
    static const uint8_t *toBGR_ptr(const cv::Mat &imgBGR);

  private:
    const std::vector<uint8_t> data;
    const size_t rows, cols;
};