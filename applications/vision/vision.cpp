#include <ANSIColors.hpp>
#include <BaremetalCommunicationDef.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <SharedMem.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <thread>

using namespace std;
using namespace chrono_literals;

string to_padded_string(int i, uint8_t n_zero = 4);

void loop();

int main() {
    while (1) {
        try {
            loop();
        } catch (std::runtime_error &e) {
            cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl;
            cerr << "Restarting loop ..." << endl;
        }
    }
}

void loop() {
    LocationFinder lf = cv::VideoCapture(0);
    cout << ANSIColors::greenb << "Camera initialization done!"
         << ANSIColors::reset << endl;

    // Print some details about the video input
    cv::VideoCapture &cap = lf.getCapture();
    int frame_width       = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height      = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps            = cap.get(cv::CAP_PROP_FPS);
    cout << "Frame dimensions: (" << frame_width << "×" << frame_height << ")\n"
         << "Frame rate      : " << fps << " fps" << std::endl;

    LocationTracker lt = {};  // Tracks the XY position of the drone

    ofstream squares("squares.csv");
    ofstream locs("locs.csv");
    ofstream sidelens("sidelens.csv");
    size_t i = 0;
    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);

        cv::imwrite(string("mask") + to_padded_string(i) + ".bmp",
                    lf.getMaskImage());
        cv::imwrite(string("image") + to_padded_string(i) + ".bmp",
                    lf.getImage());

        squares << i << ",";
        for (auto point : lf.getSquare().points)
            if (point)
                squares << point->x << "," << point->y << ",";
        squares << endl;
        locs << i << "," << location.x << "," << location.y << endl;
        sidelens << i << "," << lf.getSideLength() << endl;
        i++;

        std::cout << "sleep" << std::endl;
        this_thread::sleep_for(100ms);

        auto duration = pt.getDuration<chrono::microseconds>();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << 1e-3 * duration << " ms → "
             << 1e6 / duration << " fps" << endl;
    }
}

string to_padded_string(int i, uint8_t n_zero) {
    string str = to_string(i);
    return string(n_zero - str.length(), '0') + str;
}