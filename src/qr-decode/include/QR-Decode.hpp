#pragma once

#include <opencv2/core/mat.hpp>
#include <vector>

namespace QR {
std::vector<uint8_t> decode(const cv::Mat &img);
}