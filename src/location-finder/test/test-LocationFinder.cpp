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
    constexpr size_t GRIDSIZE = 48;

    LocationFinder lf = imagePath;
    LocationTracker lt;

    cv::VideoCapture &video = lf.getCapture();
    int frame_width         = int(video.get(3));
    int frame_height        = int(video.get(4));
    double fps              = video.get(cv::CAP_PROP_FPS) / 4;
    cv::VideoWriter out     = {filename + ".out.mp4",
                           cv::VideoWriter::fourcc('M', 'P', '4', 'V'), fps,
                           cv::Size{frame_width * 3, frame_height}};

    constexpr float alpha = 0.99;
    constexpr float beta  = 0.0;

    try {
        cv::Mat locimg;
        size_t framectr = 0;
        while (true) {
            Point locInSquare = lf.updateLocation();
            Point location    = lt.update(locInSquare);
            Square square     = lf.getSquare();
            cv::Mat maskimgrgb;
            cv::cvtColor(lf.getMaskImage(), maskimgrgb, cv::COLOR_GRAY2BGR);
            cv::circle(
                maskimgrgb,
                cv::Point(lf.getSquareCenter().x, lf.getSquareCenter().y), 5,
                cv::Scalar(255, 0, 255), -1);
            for (auto &p : lf.getSquare().points)
                if (p)
                    cv::circle(maskimgrgb, cv::Point(p->x, p->y), 5,
                               cv::Scalar(0, 255, 0), -1);
            cv::Mat image = lf.getImage();

            std::array<cv::Scalar, 5> colors = {{
                {0, 80, 255},
                {0, 200, 255},
                {0, 255, 0},
                {255, 255, 0},
                {255, 150, 0},
            }};
            size_t c                         = 0;
            for (auto &line : square.lines) {
                auto &color = colors[c++];
                if (line) {
                    cv::Point p1 = {
                        int(line->lineCenter.x),
                        int(line->lineCenter.y),
                    };
                    cv::Point p2 = {
                        int(line->lineCenter.x) +
                            int(round(lf.getSideLength() * line->angle.cosf())),
                        int(line->lineCenter.y) +
                            int(round(lf.getSideLength() * line->angle.sinf())),
                    };
                    cv::line(image, p1, p2, color, 3);
                }
            }

            cv::Mat outimg;
            cv::hconcat(image, maskimgrgb, outimg);
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
            cv::Point point = {
                int(std::round(size.width / 2.0f + location.x * GRIDSIZE)),
                int(std::round(size.height / 2.0f - location.y * GRIDSIZE)),
            };
            cv::circle(locimg, point, 3, cv::Scalar(255, 0, 0), -1);

            auto font    = cv::FONT_HERSHEY_SIMPLEX;
            auto locimgo = locimg.clone();
            auto color   = cv::Scalar(255, 128, 128);
            for (size_t y = 0; y < size.height / GRIDSIZE + 1; ++y) {
                size_t yy = y * GRIDSIZE + (size.height / 2) % GRIDSIZE;
                cv::line(locimgo, cv::Point(0, yy), cv::Point(size.width, yy),
                         color, 1);
            }
            for (size_t x = 0; x < size.width / GRIDSIZE + 1; ++x) {
                size_t xx = x * GRIDSIZE + (size.width / 2) % GRIDSIZE;
                cv::line(locimgo, cv::Point(xx, 0), cv::Point(xx, size.height),
                         color, 1);
            }
            cv::putText(locimgo, std::to_string(++framectr),
                        cv::Point(16, size.height - 16), font, 1,
                        cv::Scalar(0, 100, 255), 2, cv::LINE_AA);
            cv::hconcat(outimg, locimgo, outimg);
            cv::cvtColor(outimg, outimg, cv::COLOR_RGB2BGR);
            out.write(outimg);
            // cout << location << endl;
        }
    } catch (std::runtime_error &e) {
        cerr << ANSIColors::red << e.what() << ANSIColors::reset << endl;
    }
}