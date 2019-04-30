#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <QR-Decode.hpp>
#include <PerfTimer.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <future>
#include <chrono>
#include <thread>

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
    LocationTracker lt;

    cv::VideoCapture &cap = lf.getCapture();
    int frame_width       = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height      = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps            = cap.get(cv::CAP_PROP_FPS);

    cout << "Frame dimensions: (" << frame_width << "×" << frame_height << ")\n"
         << "Frame rate      : " << fps << " fps" << std::endl;

    cv::Mat locimg;
    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);
        
        // qr code moet enkel gedecodeerd worden op locatie waar er een verwacht wordt (afhankelijk van welke state)
        // als decoderen gelukt is moet state hier ook aangepast worden 
        cv::Mat imgbgr = lf.getImage(), img;
        cv::cvtColor(imgbgr, img, cv::COLOR_BGR2RGB);
        future<string> willBeDecoded = async(launch::async, QR::decode, img.clone());

        // niet zeker van deze lijn
        while(willBeDecoded.wait_for(chrono::seconds(0)) != future_status::ready){}
        string decoded = willBeDecoded.get();

        

        auto duration     = pt.getDuration();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << duration << " µs → " << 1e6 / duration
             << " fps" << endl;
    }
}