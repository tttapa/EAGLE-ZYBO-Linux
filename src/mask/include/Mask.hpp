#pragma once

#include <Pixel.hpp>
#include <opencv2/core/mat.hpp>

class Mask {
  public:
    /**
     * @brief   Construct a new Mask object from a plain array of interleaved 
     *          RGB bytes.
     * 
     * @param   imgRGB
     *          A pointer to an array of interleaved RGB bytes.
     * @param   rows 
     *          The height of the image, or the number of rows in the pixel 
     *          matrix.
     * @param   cols 
     *          The width of the image, or the number of columns in the pixel
     *          matrix.
     */
    Mask(const uint8_t *imgRGB, uint rows, uint cols);
    /**
     * @brief   Construct a new Mask object from an OpenCV image.
     * 
     * @param   imgRGB 
     *          An OpenCV image.
     */
    Mask(const cv::Mat &imgRGB);
    /**
     * @brief   Default constructor.
     */
    Mask() : data{}, rows{0}, cols{0} {}
#if 0
    /**
     * @brief   Default copy constructor.
     * @todo    Do I need the explicit default?
     */
    Mask(const Mask &) = default;
    /**
     * @brief   Default move constructor.
     * @todo    Do I need the explicit default?
     */
    Mask(Mask &&) = default;

    /**
     * @brief   Default copy assignment.
     * @todo    Do I need the explicit default?
     */
    Mask &operator=(const Mask &) = default;
    /**
     * @brief   Default move assignment.
     * @todo    Do I need the explicit default?
     */
    Mask &operator=(Mask &&) = default;
#endif

    /**
     * @brief   Get the value of the mask at a given location.
     * 
     * @param   px 
     *          The pixel to check.
     * 
     * @return  0xFF
     *          The pixel was red.
     * @return  0x00
     *          The pixel was not red. 
     */
    uint8_t get(Pixel px) const {
        if (!px.inRange(cols, rows))
            throw std::out_of_range(__PRETTY_FUNCTION__);  // TODO: I'm lazy
        return data[px.x + (size_t) cols * px.y];
    }

    /**
     * @brief   Set the value of the mask at the given location.
     * 
     * @param   px 
     *          The pixel to set the value of.
     * @param   value
     *          The value to set the pixel to.
     */
    void set(Pixel px, uint8_t value = 0xFF) {
        if (!px.inRange(cols, rows))
            throw std::out_of_range(__PRETTY_FUNCTION__);  // TODO: I'm lazy
        data[px.x + (size_t) cols * px.y] = value;
    }

    /// Get the width of the mask in pixels.
    uint width() const { return cols; }
    /// Get the height of the mask in pixels.
    uint height() const { return rows; }

    /// Get a pointer to the array of mask bytes.
    const uint8_t *ptr() const { return data.data(); }
    uint8_t *ptr() { return data.data(); }

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

    /**
     * @brief   Generate the black and white mask of the given image.
     *          
     * @param   imgRGB 
     *          A pointer to an array of interleaved RGB bytes.
     * @param   numberOfPixels 
     *          The total number of pixels in the array (rows × columns).
     * @return  A vector of bytes with either a value of 0xFF for red pixels,
     *          or a value off 0x00 for any other color.
     */
    static std::vector<uint8_t> generateMask(const uint8_t *imgRGB,
                                             size_t numberOfPixels);

    /**
     * @brief   Check if an OpenCV matrix has the correct image type:
     *          The matrix should have two dimensions, 
     *          the type should be @c CV_8U, 
     *          it should have 3 channels (RGB), 
     *          and it should be stored in memory contiguously.
     */
    static bool checkImageType(const cv::Mat &img);
    /**
     * @brief   Ensure that an OpenCV matrix has the correct image type, and 
     *          return a pointer to its data.
     * 
     * @throws  runtime_error
     *          The image type is incorrect.
     * 
     * @see     checkImageType
     */
    static const uint8_t *toRGB_ptr(const cv::Mat &imgRGB);

    const bool hasQrCode(const cv::Mat imgRGB);

  private:
    std::vector<uint8_t> data;
    uint rows, cols;
};