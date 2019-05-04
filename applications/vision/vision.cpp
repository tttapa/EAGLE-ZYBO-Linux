#include <ANSIColors.hpp>
#include <BaremetalCommunicationDef.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <SharedMem.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

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
    LocationFinder lf = cv::VideoCapture(0);

    cv::VideoCapture &cap = lf.getCapture();
    int frame_width       = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height      = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps            = cap.get(cv::CAP_PROP_FPS);

    cout << "Frame dimensions: (" << frame_width << "×" << frame_height << ")\n"
         << "Frame rate      : " << fps << " fps" << std::endl;

    LocationTracker lt;
    SharedMemory<VisionCommStruct> visionComm;

    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);
        auto duration     = pt.getDuration();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << duration << " µs → " << 1e6 / duration
             << " fps" << endl;

        if (visionComm->isDoneReading())
            visionComm->write({
                {location.x, location.y},
                lf.getAngle().rad(),
            });
        else
            cerr << ANSIColors::magenta
                 << "Warning: Baremetal didn't read previous vision data"
                 << ANSIColors::reset << endl;
    }
}