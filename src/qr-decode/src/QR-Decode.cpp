#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace QR {

using namespace cv;
using namespace std;

vector<uint8_t> decode(const Mat &img) {
    QRCodeDetector qrdec;
    vector<Point2f> points;
    if (!qrdec.detect(img, points))
        throw runtime_error("Unable to detect QR code");
    std::cout << points << std::endl;
    string result = qrdec.decode(img, points);
    // string result = qrdec.detectAndDecode(img);
    if (result.empty())
        throw runtime_error("Unable to decode QR code");
    return {result.begin(), result.end()};
}

}  // namespace QR