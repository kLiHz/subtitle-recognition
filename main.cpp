#include <iostream>
#include <string>
#include <filesystem>
#include <memory>
#include <functional>

#include "boost/program_options.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "fmt/core.h"
#include "tesseract/baseapi.h"

namespace po = boost::program_options;
namespace fs = std::filesystem;

std::string msToTimeStr(long long t) {
    auto ms = t % 1000;
    auto s = t / 1000;
    auto m = s / 60;
    auto h = m / 60;
    m %= 60;
    s %= 60;
    return fmt::format("{:0>2}:{:0>2}:{:0>2},{:0>3}", h, m, s, ms);
}

class SkipVideoCapture : public cv::VideoCapture {
public:
    explicit SkipVideoCapture(const cv::String& filename) : cv::VideoCapture(filename) {}
    auto getFPS() { return this->get(cv::VideoCaptureProperties::CAP_PROP_FPS); }
    auto getPosMilliseconds() { return this->get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC); }
    auto skip(double t) {
        auto n = static_cast<long long>(t * this->getFPS());
        while (n > 0) {
            this->grab();
            n -= 1;
        }
    }
};

int main(int argc, char * argv[]) {
    fs::path input_file;
    fs::path tessdata_dir;
    cv::Rect box;
    double gap;
    double duration;
    std::string lang;
    int threshold;
    int min_conf;

    try {
        std::string input;
        std::string dir;
        std::vector<int> b;

        po::options_description desc("Video Subtitle OCR v0.0.1");

        desc.add_options()
                ("help,h", "Print help message.")
                ("search-gap,g",po::value<double>(&gap)->default_value(0.16), "Gap seconds between frames when reading from video.")
                ("min-duration,m",po::value<double>(&duration)->default_value(0.5), "Minimum duration of each subtitle, in seconds.")
                ("threshold,t",po::value<int>(&threshold)->default_value(239), "Threshold value used to binarize image.")
                ("min-conf,c",po::value<int>(&min_conf)->default_value(85), "Minimum confidence value of OCR result.")
                ("input-file,i",po::value<std::string>(&input)->required(), "Input file to be processed.")
                ("box,b",po::value<std::vector<int>>(&b)->multitoken()->composing()->required(), "Text detection box in (x, y, w, h).")
                ("lang,l",po::value<std::string>(&lang)->required(), "Language configuration string (same as tesseract's LANG).")
                ("tessdata-dir,d",po::value<std::string>(&dir)->required(), "Path to tessdata directory.")
                ;

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        po::notify(vm);


        if (b.size() < 4) {
            std::cerr << "Box must be specified as (x, y, w, h).";
            return 1;
        }

        box = cv::Rect(b[0], b[1], b[2], b[3]);

        input_file = fs::path(input);
        tessdata_dir = fs::path(dir);
    }
    catch (const boost::program_options::error& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    std::cout << "Input file: " << input_file.string() << "\n";
    std::cout << "Detection box: " << box << "\n";
    std::cout << "Gap seconds: " << gap << "\n";

    auto i = SkipVideoCapture(input_file.string());

    if (!i.isOpened()) {
        std::cout << "Cannot open video file." << "\n";
        return 1;
    }

    cv::namedWindow("show");

    cv::Mat frame;
    cv::Mat bin;
    cv::Mat t;

    std::string prev_text;
    long long prev_ms;
    long long cnt = 0;

    using TessAPI = tesseract::TessBaseAPI;
    using UniqueTessAPI = std::unique_ptr<TessAPI ,std::function<void(TessAPI *)>>;
    auto api = UniqueTessAPI(new TessAPI(), [](TessAPI * p){
        p->End();
        delete p;
    });
    api->Init(tessdata_dir.string().c_str(), lang.c_str());

    while (i.isOpened()) {
        if (!i.grab()) { break; }
        i.retrieve(frame);

        frame(box).copyTo(t);

        cv::cvtColor(t, t, cv::ColorConversionCodes::COLOR_BGR2GRAY);
        cv::medianBlur(t, t, 3);
        cv::threshold(t, t, threshold, 255, cv::ThresholdTypes::THRESH_BINARY_INV);

        auto ms = i.getPosMilliseconds();

        api->SetImage(t.data, t.cols, t.rows, 1, t.step);
        auto conf = api->MeanTextConf();
        auto p = std::unique_ptr<char[]>(api->GetUTF8Text());
        std::string text = p.get();

        if (conf > min_conf || text.empty()) {
            if (text != prev_text) {
                if (!prev_text.empty()) {
                    cnt += 1;
                    std::cout
                            << std::to_string(cnt) << "\n"
                            << msToTimeStr(prev_ms) << " --> " << msToTimeStr(ms) << "\n"
                            << prev_text << "\n";
                    i.skip(duration);
                }
                prev_text = std::move(text);
                prev_ms = ms;
            }
        }

        cv::putText(t, std::to_string(conf), {40,40}, cv::FONT_HERSHEY_PLAIN, 1, {0});
        cv::imshow("show", t);
        auto k = cv::waitKey(30);
        if (k == 27) { break; }

        i.skip(gap);
    }

    return 0;
}
