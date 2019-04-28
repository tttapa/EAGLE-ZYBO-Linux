#include <PerfTimer.hpp>
#include <QR-Decode.hpp>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

using namespace std;

TEST(QRDecoder, fromImage) {
#ifdef ZYBO
    string imagePath = "/media/QR.bmp";
#else
    string imagePath = string(getenv("WORKSPACE_ROOT")) + "/test/QR.bmp";
#endif
    cout << "Reading image `" << imagePath << "`" << endl;
    cv::Mat imgbgr = cv::imread(imagePath);
    cv::Mat img;
    cv::cvtColor(imgbgr, img, cv::COLOR_BGR2RGB);
    PerfTimer pt;
    string decoded = QR::decode(img);
    cout << "QR decoding took " << pt.getDuration<chrono::microseconds>()
         << "µs" << endl;
    string expected = "AAAAAAAAAAAAAAAAAAAACQMNCQACE2liv6B2BEyi97naxRwB/"
                      "JPBBaQTuRTjQmJnhwscT0fXX9krwoCPjg==";
    ASSERT_EQ(decoded, expected);
}