#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>

using namespace std;
using namespace cv;

class Card {

public:
    string name;
    Mat image;
    vector <KeyPoint> keypoints;
    Mat descriptor;

    Card(string image_path, string name_) {
        name = name_;
        image = imread(image_path);
        Ptr<Feature2D> sift = SIFT::create();
        sift->detectAndCompute(image, cv::noArray(), keypoints, descriptor);
    }
};

void rot(cv::RotatedRect& box, cv::Mat& image, cv::Mat& cropped);

vector <vector <Point>> get_contours(Mat source_img, Size kernel_size);

string get_name(Mat img, vector <Card> etalons);

int main()
{
    vector <Card> cards;
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/9_clubs.jpg", "9 of Clubs"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/K_diamonds.jpg", "King of Diamonds"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/A_hearts.jpg", "Ace of Hearts"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/Q_spades.jpg", "Queen of Spades"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/J_hearts.jpg", "Jack of Hearts"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/6_spades.jpg", "6 of Spades"));
    cards.push_back(Card("D:/lab1os/carty_nardy_domino/A_diamonds.jpg", "Ace of Diamonds"));

    /*cv::VideoCapture cap("D:/lab1os/carty_nardy_domino/videli_video.mp4");
    if (!cap.isOpened()) {
        std::cout << "Error: could not open video writer" << std::endl;
        return -1;
    }

    std::vector<cv::Mat> frames;
    int frame_count = 0;
    while (true) {
        cv::Mat test;
        cap >> test;
        if (test.empty()) break;*/

        Mat test = imread("D:/lab1os/carty_nardy_domino/test2.jpg");
        //resize(test, test, cv::Size(), 0.25, 0.25);
        Mat clone = test.clone();
        //vector <vector <Point>> contours = get_contours(test, Size(3, 3));
        Mat  gauss_img, edges_img;
        GaussianBlur(test, gauss_img, cv::Size(13, 13), 0);
        Canny(gauss_img, edges_img, 100, 150);
        vector <Vec4i> hierarchy;
        vector <vector <Point>> contours;
        findContours(edges_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (size_t i = 0; i < contours.size(); i++) {
            double epsilon = 0.02 * arcLength(contours[i], true);
            std::vector<cv::Point> approx;
            approxPolyDP(contours[i], approx, epsilon, true);

            cv::RotatedRect box = cv::minAreaRect(approx);

            cv::Mat img;
            rot(box, test, img);
            if (img.empty()) {
                continue;
            }
            std::string name = get_name(img, cards);

            if (approx.size() == 4 && cv::isContourConvex(approx)) {
                cv::Scalar color = cv::Scalar(0, 255, 0);
                drawContours(clone, contours, (int)i, color, 1, cv::LINE_8, hierarchy, 0);
                cv::Moments M = moments(approx);
                cv::Point center(M.m10 / M.m00, M.m01 / M.m00);
                putText(clone, name, center, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.65, color, 1);
            }
        }
        //frames.push_back(clone);
        imshow("", clone);
        waitKey(0);

        destroyAllWindows();
    //}
    //cap.release();


    //cv::VideoWriter video("D:/lab1os/carty_nardy_domino/ne_videli.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size(480, 360));
    //if (!video.isOpened()) {
    //    std::cout << "Error: could not open video writer" << std::endl;
    //    return -1;
    //}

    //for (int i = 0; i < frames.size(); i++) {
    //    cv::imshow("video", frames[i]);
    //    cv::Mat buff = frames[i].clone();
    //    cv::resize(buff, buff, cv::Size(480, 360));
    //    video << buff;

    //    char c = (char)cv::waitKey(30);
    //    if (c == 27) break;
    //}

    //cv::destroyAllWindows();
    //video.release();

    return 0;
}

void rot(cv::RotatedRect& box, cv::Mat& image, cv::Mat& cropped) {
    cv::Mat rotated;

    cv::Size rect_size = box.size;
    if (box.angle < -45.) {
        std::swap(rect_size.width, rect_size.height);
        box.angle += 90.0;
    }

    cv::Mat M = cv::getRotationMatrix2D(box.center, box.angle, 1.0);
    cv::warpAffine(image, rotated, M, image.size(), cv::INTER_CUBIC);

    cv::getRectSubPix(rotated, rect_size, box.center, cropped);
    if (cropped.size().width > cropped.size().height) {
        cv::rotate(cropped, cropped, cv::ROTATE_90_CLOCKWISE);
    }
}

vector <vector <Point>> get_contours(Mat source_img, Size kernel_size) {
    //resize(source_img, source_img, cv::Size(), 0.5, 0.5);
    Mat  gauss_img, edges_img;
    GaussianBlur(source_img, gauss_img, cv::Size(11, 11), 0);
    Canny(gauss_img, edges_img, 100, 150);
    vector <Vec4i> hierarchy;
    vector <vector <Point>> contours;
    findContours(edges_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    return contours;
}

string get_name(Mat img, vector <Card> etalons) {
    cv::Ptr<cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();

    cv::Ptr<cv::Feature2D> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> kp;
    cv::Mat dis;
    sift->detectAndCompute(img, cv::noArray(), kp, dis);

    double min_dist = DBL_MAX;
    int min_dist_idx = -1;

    for (size_t i = 0; i < etalons.size(); ++i) {
        std::vector<cv::DMatch> matches;
        matcher->match(dis, etalons[i].descriptor, matches);

        double dist = 0;
        for (size_t j = 0; j < matches.size(); ++j) {
            dist += matches[j].distance;
        }
        dist /= matches.size();

        if (dist < min_dist) {
            min_dist = dist;
            min_dist_idx = i;
        }
    }

    if (min_dist_idx != -1) {
        return etalons[min_dist_idx].name;
    }
    else {
        return "Unknown";
    }
}
