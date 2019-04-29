#include <GridFinder.hpp>
#include <LocationFinder.hpp>
#include <Mask.hpp>
#include <PerfTimer.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using std::string;

// TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }

TEST(MaskGridFinder, fromImage) {
#ifdef ZYBO
    string imagePath = "/media/TEST-LocationFinder_getLocation.bmp";
#else
    string imagePath = string(getenv("WORKSPACE_ROOT")) +
                       "/test/TEST-LocationFinder_getLocation.bmp";
#endif
    std::cout << "Reading image `" << imagePath << "`" << std::endl;
    cv::Mat imgbgr = cv::imread(imagePath);
    cv::Mat img;
    cv::cvtColor(imgbgr, img, cv::COLOR_BGR2RGB);
    PerfTimer pt;
    Mask mask = img;
    std::cout << "Masking took " << pt.getDuration<std::chrono::microseconds>()
              << "µs" << std::endl;
    cv::Mat maskimg = {img.rows, img.cols, CV_8UC1, mask.ptr()};
    cv::imwrite("mask.bmp", maskimg);
    GridFinder gf = std::move(mask);
    pt            = PerfTimer();
    Square sq     = gf.findSquare();
    std::cout << "GridFinder took "
              << pt.getDuration<std::chrono::microseconds>() << "µs"
              << std::endl;
    std::cout << sq << std::endl;
    // TODO: check square result!
}