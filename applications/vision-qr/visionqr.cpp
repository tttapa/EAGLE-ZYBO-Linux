#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <QRCryptoManager.hpp>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

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
    // Open camera 0
    LocationFinder lf = cv::VideoCapture(0);

    // Print some details about the video input
    cv::VideoCapture &cap = lf.getCapture();
    int frame_width       = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height      = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps            = cap.get(cv::CAP_PROP_FPS);
    cout << "Frame dimensions: (" << frame_width << "×" << frame_height << ")\n"
         << "Frame rate      : " << fps << " fps" << std::endl;

    LocationTracker lt;  // Tracks the XY position of the drone
    BaremetalShared<BaremetalCommStruct> baremetal;
    QRCryptoManager qrCryptoMgr = baremetal;

    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);

        qrCryptoMgr.update(lf.getImage());

        auto duration = pt.getDuration<chrono::microseconds>();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << 1e-3 * duration << " ms → "
             << 1e6 / duration << " fps" << endl;
    }
}