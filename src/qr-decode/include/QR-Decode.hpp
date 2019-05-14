#pragma once

#include <opencv2/core/mat.hpp>
#include <string>

namespace QR {
/**
 * @brief   Try to detect and decode the QR code in the given RGB image.
 * 
 * @param   img
 *          An RGB image.
 * @return  The data of the QR code, or an empty string if no QR code was found. 
 */
std::string decode(const cv::Mat &img);
}