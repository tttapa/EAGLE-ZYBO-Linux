#include <PerfTimer.hpp>
#include <QR-Decode.hpp>
#include <future>
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

TEST(QRDecoder, async) {
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
    future<string> willBeDecoded =
        async(launch::async, QR::decode, img.clone());
    /* Clear the image, to make sure that
    the other thread still succeeds when
    the original image is updated */
    img = cv::Mat();
    cout << "Spawning QR thread took " << pt.getDuration<chrono::microseconds>()
         << "µs" << endl;
    // Do other stuff here while the QR is being decoded ...
    string decoded = willBeDecoded.get();  // Wait for the QR decoding to finish
    /* In the actual vision code, you would check wait_for(chrono::seconds(0))
    == future_status::ready on each iteration instead of get()ing the value 
    immediately. */
    cout << "QR decoding took " << pt.getDuration<chrono::microseconds>()
         << "µs" << endl;
    string expected = "AAAAAAAAAAAAAAAAAAAACQMNCQACE2liv6B2BEyi97naxRwB/"
                      "JPBBaQTuRTjQmJnhwscT0fXX9krwoCPjg==";
    ASSERT_EQ(decoded, expected);
}