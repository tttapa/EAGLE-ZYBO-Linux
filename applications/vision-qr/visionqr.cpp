#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <Logger.hpp>
#include <PerfTimer.hpp>
#include <QRCryptoManager.hpp>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>  // usleep

using namespace std;
using namespace chrono_literals;

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
    // Initialize communication with Bare-metal
    SharedMemory<VisionCommStruct> visionComm;
    QRCryptoManager qrCryptoMgr;

    cout << "Waiting for Baremetal to be initialized ..." << endl;
    while (!visionComm->isInitialized())
        usleep(10'000);
    cout << ANSIColors::greenb << "Baremetal initialization done!"
         << ANSIColors::reset << endl;

    // Initialize the logger
    Logger logger("239.0.0.2", 5003);
    logger.begin();
    promise<void> loggerStop;
    // And start it in a new thread, updating every 5ms, until loggerStop is set
    thread loggerThread(
        [&logger](auto loggerStop) {
            while (loggerStop.wait_for(5ms) != future_status::ready)
                logger.update();
        },
        move(loggerStop.get_future()));

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

    LocationTracker lt;  // Tracks the XY position of the drone

    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);

        qrCryptoMgr.update(lf.getImage());

        auto duration = pt.getDuration<chrono::microseconds>();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << 1e-3 * duration << " ms → "
             << 1e6 / duration << " fps" << endl;

        if (locInSquare) {  // If we have a new position measurement
            if (visionComm->isDoneReading()) {
                visionComm->write({
                    {location.x, location.y},
                    lf.getAngle().rad(),
                });
            } else {
                cerr << ANSIColors::magenta
                     << "Warning: Baremetal didn't read previous vision data"
                     << ANSIColors::reset << endl;
            }
        }
    }

    loggerStop.set_value();  // Tell the logger thread to stop
    loggerThread.join();     // TODO: is this necessary?
}