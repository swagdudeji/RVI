#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void init_tuning_window(string window_name, int width, int height, vector <int> & low_tuning, vector <int> & high_tuning);
void pre_process_frame(Mat frame, Mat& out_frame, Mat& mask, vector <int>& low_tuning, vector <int>& high_tuning);
void draw_contour(Mat& frame, vector <Point> contour, int area_limit);

int main() {
    VideoCapture cap("C:/Users/sidja/Desktop/screamer.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Error\n";
        return -1;
    }

    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    namedWindow("Raspakouka", WINDOW_NORMAL);
    resizeWindow("Raspakouka", frame_width, frame_height);
    vector <int> low_tuning{ 65, 16, 140 };
    vector <int> high_tuning{ 255, 50, 255 };
    init_tuning_window("Screamer", frame_width, frame_height, low_tuning, high_tuning);

    VideoWriter video("C:/Users/sidja/Desktop/outpuk.mp4", VideoWriter::fourcc('X', 'V', 'I', 'D'), 30, Size(frame_width, frame_height));
    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;

        Mat edges, mask;
        pre_process_frame(frame, edges, mask, low_tuning, high_tuning);
        vector <vector <Point>> contours;
        cv::findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (vector <Point> contour : contours){
            draw_contour(frame, contour, 1000);
        }

        cv::imshow("Raspakouka", frame);
        cv::imshow("Screamer", mask);
        video.write(frame);
        if (waitKey(10) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    video.release();

    return 0;
}


void init_tuning_window(string window_name, int width, int height, vector <int>& low_tuning, vector <int>& high_tuning) {
    namedWindow(window_name, WINDOW_NORMAL);
    resizeWindow(window_name, width, height);
    createTrackbar("Low H", window_name, &low_tuning[0], 255);
    createTrackbar("Low S", window_name, &low_tuning[1], 255);
    createTrackbar("Low V", window_name, &low_tuning[2], 255);
    createTrackbar("High H", window_name, &high_tuning[0], 255);
    createTrackbar("High S", window_name, &high_tuning[1], 255);
    createTrackbar("High V", window_name, &high_tuning[2], 255);
}

void pre_process_frame(Mat frame, Mat& out_frame, Mat& mask, vector <int>& low_tuning, vector <int>& high_tuning) {
    GaussianBlur(frame, frame, Size(5, 5), 0);
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    Scalar lower_white = Scalar(low_tuning[0], low_tuning[1], low_tuning[2]);
    Scalar upper_white = Scalar(high_tuning[0], high_tuning[1], high_tuning[2]);
    inRange(hsv, lower_white, upper_white, mask);
    Canny(mask, out_frame, 30, 90);
    dilate(out_frame, out_frame, Mat(), Point(-1, -1), 2);
    cv::erode(out_frame, out_frame, Mat(), Point(-1, -1), 2);
}

void draw_contour(Mat& frame, vector <Point> contour, int area_limit) {
    double area = contourArea(contour);
    if (area > area_limit) {
        RotatedRect rotatedRect = minAreaRect(contour);
        Point2f vertices[4];
        rotatedRect.points(vertices);
        for (int j = 0; j < 4; ++j) {
            line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 2);
        }
    }
}