#include <iostream>
#include <opencv2/opencv.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <zbar.h>
#pragma GCC diagnostic pop

namespace QR {

using namespace cv;
using namespace std;
using namespace zbar;

string decode(const Mat &img) {
#if 0
    QRCodeDetector qrdec;
    vector<Point2f> points;
    if (!qrdec.detect(img, points))
        throw runtime_error("Unable to detect QR code");
    std::cout << points << std::endl;
    string result = qrdec.decode(img, points);
    // string result = qrdec.detectAndDecode(img);
    if (result.empty())
        throw runtime_error("Unable to decode QR code");
    return result;
#else
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    Image image(gray.cols, gray.rows, "Y800", gray.ptr(),
                gray.cols * gray.rows);

    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    scanner.scan(image);

    // for (Image::SymbolIterator symbol = image.symbol_begin();
    //      symbol != image.symbol_end(); ++symbol) {
    //     // do something useful with results
    //     cout << "decoded " << symbol->get_type_name() << " symbol \""
    //          << symbol->get_data() << '"' << endl;
    // }
    Image::SymbolIterator symbol = image.symbol_begin();
    string result                = symbol->get_data();
    image.set_data(NULL, 0);  // clean up
    return result;
#endif
}

}  // namespace QR