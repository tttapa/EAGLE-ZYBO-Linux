#pragma once

#include <opencv2/core/mat.hpp>
#include <string>

namespace QR {
std::string decode(const cv::Mat &img);
}