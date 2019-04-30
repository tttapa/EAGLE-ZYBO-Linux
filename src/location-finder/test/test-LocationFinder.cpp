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
    volatile size_t loop = 1;
    Mask mask            = img;
    for (size_t i = 0; i < loop - 1; ++i)
        mask = img;
    std::cout << "Masking took "
              << pt.getDuration<std::chrono::microseconds>() / loop << "µs"
              << std::endl;
    cv::Mat maskimg = {img.rows, img.cols, CV_8UC1, mask.ptr()};
    cv::imwrite("mask.bmp", maskimg);
    GridFinder gf = std::move(mask);
    pt.reset();
    Square sq     = gf.findSquare();
    std::cout << "GridFinder took "
              << pt.getDuration<std::chrono::microseconds>() << "µs"
              << std::endl;
    std::cout << sq << std::endl;
    std::cout << "Square angle = " << sq.getAngle() << std::endl;
    // TODO: check square result!
}

TEST(LocationFinder, fromSquare) {
    Square sq = {{std:nullopt,std:nullopt,std:nullopt,std:nullopt,std:nullopt},
                {std:nullopt,std:nullopt,std:nullopt,std:nullopt}}
    Point test_location = LocationFinder::test1(sq);
    std::cout << "x= " << test_location.x << "  y= " << test_location.y << std::endl;
    ASSERT_NEAR(test_location.x , 0.5, 0.02);
    ASSERT_NEAR(test_location.y , 0.5, 0.02);
    Square sq = {{{{290,140},10,0_deg},{{290,140},10,0_deg},{{220,160},10,90_deg},
                {{220,340},10,0_deg},{{420,340},10,90_deg}},
                {{220,140},{420,140},{420,340},{220,340}}}
    Point test_location = LocationFinder::test1(sq);
    std::cout << "x= " << test_location.x << "  y= " << test_location.y << std::endl;
    ASSERT_NEAR(test_location.x , 0.5, 0.02);
    ASSERT_NEAR(test_location.y , 0.5, 0.02);
}
