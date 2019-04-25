#include <PerfTimer.hpp>
#include <QR-Decode.hpp>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

using namespace std;

TEST(QRDecoder, fromImage) {
#ifdef ZYBO
    string imagePath = "/media/QR.bmp";
#else
    string imagePath = string(getenv("WORKSPACE_ROOT")) +
                       "/test/QR.bmp";
#endif
    cout << "Reading image `" << imagePath << "`" << endl;
    cv::Mat imgbgr = cv::imread(imagePath);
    cv::Mat imggrey;
    cv::cvtColor(imgbgr, imggrey, cv::COLOR_BGR2GRAY);
    PerfTimer pt;
    vector<uint8_t> decoded = QR::decode(imggrey);
    cout << "QR decoding took " << pt.getDuration<chrono::microseconds>()
         << "µs" << endl;
    string str = {decoded.begin(), decoded.end()};
    cout << "Data = " << str << endl;
}