#include <GridFinder.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
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
    Square sq = gf.findSquare();
    std::cout << "GridFinder took "
              << pt.getDuration<std::chrono::microseconds>() << "µs"
              << std::endl;
    std::cout << sq << std::endl;
    std::cout << "Square angle = " << sq.getAngle() << std::endl;
    // TODO: check square result!
}

TEST(MaskGridFinder, fromVideo) {
    string filename = "DroneCam-Spinning";
#ifdef ZYBO
    string imagePath = "/media/" + filename + ".mp4";
#else
    string imagePath =
        string(getenv("WORKSPACE_ROOT")) + "/Video/" + filename + ".mp4";
#endif
    constexpr float MAX_LOC = 6;

    LocationFinder lf = imagePath;
    LocationTracker lt;

    cv::VideoCapture &video = lf.getCapture();
    int frame_width         = int(video.get(3)) * 3;
    int frame_height        = int(video.get(4));
    double fps              = video.get(cv::CAP_PROP_FPS) / 1;
    cv::VideoWriter out     = {filename + ".out.avi",
                           cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps,
                           cv::Size{frame_width, frame_height}};

    constexpr float alpha = 0.99;
    constexpr float beta  = 0.0;

    try {
        cv::Mat locimg;
        while (true) {
            Point locInSquare = lf.getLocation();
            Point location    = lt.update(locInSquare);
            cv::Mat maskimgrgb;
            cv::cvtColor(lf.getMaskImage(), maskimgrgb, cv::COLOR_GRAY2BGR);
            cv::Mat outimg;
            cv::hconcat(lf.getImage(), maskimgrgb, outimg);
            cv::Size size = lf.getImage().size();
            if (locimg.empty())
                locimg = {size, lf.getImage().type(), cv::Scalar(0)};
            else
                for (int y = 0; y < locimg.rows; y++) {
                    for (int x = 0; x < locimg.cols; x++) {
                        for (int c = 0; c < 3; c++) {
                            locimg.at<cv::Vec3b>(y, x)[c] =
                                cv::saturate_cast<uint8_t>(
                                    alpha * (locimg.at<cv::Vec3b>(y, x)[c]) +
                                    beta);
                        }
                    }
                }
            int mindim      = std::min(size.height, size.width);
            cv::Point point = {
                int(std::round(size.width / 2.0f +
                               mindim / 2.0f * location.x / MAX_LOC)),
                int(std::round(size.height / 2.0f -
                               mindim / 2.0f * location.y / MAX_LOC)),
            };
            cv::circle(locimg, point, 3, cv::Scalar(255, 0, 0), -1);
            cv::hconcat(outimg, locimg, outimg);
            cv::cvtColor(outimg, outimg, cv::COLOR_RGB2BGR);
            out.write(outimg);
            cout << location << endl;
        }
    } catch (std::runtime_error &e) {
        cerr << ANSIColors::red << e.what() << ANSIColors::reset << endl;
    }
}