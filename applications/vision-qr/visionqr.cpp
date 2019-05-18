#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <QRCryptoManager.hpp>
#include <ThreadedLogger.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>  // usleep

using namespace std;
using namespace chrono_literals;

#define DEBUG_VISION

string to_padded_string(int i, uint8_t n_zero = 4);

void loop();

int main() {
    while (true)
        try {
            loop();
        } catch (std::runtime_error &e) {
            cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl
                 << "Restarting loop ..." << endl;
        }
}

void loop() {
    // Initialize communication with Bare-metal
    SharedMemory<VisionCommStruct> visionComm;
    QRCryptoManager qrCryptoMgr;

    cout << "Waiting for Baremetal to be initialized ..." << endl;
    while (!visionComm->isInitialized())
        usleep(10'000);
    cout << ANSIColors::greenb << "Baremetal initialization done!"
         << ANSIColors::reset << endl;

    // Initialize the logger
    ThreadedLogger logger = Logger{{
        {"239.0.0.2", 5003},      // Multicast
        {"192.168.4.100", 5001},  // Unicast
    }};
    cout << "logger : " << logger.isInitialized() << endl;
    // Make sure that Baremetal has initialized the logger
    logger.checkInitialized();
    // Spawns another thread, is cleaned up when destructed. Update every 17ms.
    logger.begin(17ms);

    // Open camera 0
    cout << "Waiting for camera to be initialized ..." << endl;
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

#ifdef DEBUG_VISION
    filesystem::path outdir = "output-vision-qr";
    filesystem::create_directories(outdir);
    filesystem::remove_all(outdir);
    ofstream squares(outdir / "squares.csv");
    ofstream locs(outdir / "locs.csv");
    ofstream sidelens(outdir / "sidelens.csv");
    size_t i = 0;
#endif
    while (true) {
        // PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);

#ifdef DEBUG_VISION
        cv::imwrite(outdir / (string("mask") + to_padded_string(i) + ".bmp"),
                    lf.getMaskImage());
        cv::imwrite(outdir / (string("image") + to_padded_string(i) + ".bmp"),
                    lf.getImage());
#endif

        qrCryptoMgr.update(lf.getImage());

        if (locInSquare) {  // If we have a new position measurement
            if (visionComm->isDoneReading()) {
                double angle            = lf.getAngle().rad();
                VisionPosition position = {location.x, location.y};
                VisionData data         = {position, angle};
                visionComm->write(data);
            } else {
                cerr << ANSIColors::magenta
                     << "Warning: Baremetal didn't read previous vision data"
                     << ANSIColors::reset << endl;
            }
        }

#ifdef DEBUG_VISION
        squares << i << ",";
        for (auto point : lf.getSquare().points)
            if (point)
                squares << point->x << "," << point->y << ",";
        squares << endl;
        locs << i << "," << location.x << "," << location.y << endl;
        sidelens << i << "," << lf.getSideLength() << endl;
        i++;
#endif

        // auto duration = pt.getDuration<chrono::microseconds>();
        static size_t subframectr = 0;
        if (subframectr++ == 30) {
            cout << "Position: " << location << endl;
            subframectr = 0;
        }
        // cout << "Vision duration: " << 1e-3 * duration << " ms → "
        //      << 1e6 / duration << " fps" << endl;
    }
}

string to_padded_string(int i, uint8_t n_zero) {
    string str = to_string(i);
    return string(n_zero - str.length(), '0') + str;
}