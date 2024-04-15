#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <iostream>


using namespace std;
using namespace cv;

Mat pre_process_image(Mat source_img, Size kernel_size);
Mat detected_shapes(Mat source_img, Mat pre_processed_img, int threshold, int ratio, int kernel_size);
string recognized_shape(vector <Point> shape);
void display_contour(Mat& source_img, vector <Point> contour, string contour_label, int font_face, int font_scale, Scalar colour, int thickness);

int main() {

    string img_path = "C:/Users/sidja/Desktop/algebra.jpg";
    Mat img = imread(img_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: " << img_path << std::endl;
        return -1;
    }

    Mat pre_processed_img = pre_process_image(img, Size(5, 5));
    Mat detected_shapes_img = detected_shapes(img, pre_processed_img, 50, 1.5, 3);

    imshow("Bylo ne ochen`", img);
    waitKey(0);
    imshow("A stalo sovsem krasota", detected_shapes_img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}

Mat pre_process_image(Mat source_img, Size kernel_size) {
    Mat grayscale, blurred;
    cvtColor(source_img, grayscale, COLOR_BGR2GRAY);
    GaussianBlur(grayscale, blurred, kernel_size, 0);
    return blurred;
}

Mat detected_shapes(Mat source_img, Mat pre_processed_img, int threshold, int ratio, int kernel_size) {
    
    vector <vector <Point>> contours;
    Mat canny_img;
    Mat result_img = source_img.clone();

    Canny(pre_processed_img, canny_img, threshold, threshold * ratio, kernel_size);
    findContours(canny_img, contours, RETR_TREE, CHAIN_APPROX_TC89_L1);

    for (vector <Point> contour : contours) {
        vector <Point> shape;
        approxPolyDP(contour, shape, cv::arcLength(contour, true) * 0.02, true);
        string recognized_shape_label = recognized_shape(shape);
        display_contour(result_img, contour, recognized_shape_label, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
    }

    return result_img;
}

string recognized_shape(vector <Point> shape) {
    int key_points = shape.size();
    string shape_label;
    if (key_points == 3)
        shape_label = "Triangle";
    else if (key_points == 4)
        shape_label = "Square";
    else
        shape_label = "Circle";

    return shape_label;
}

void display_contour(Mat & source_img, vector <Point> contour, string contour_label, int font_face, int font_scale, Scalar colour, int thickness) {
    cv::drawContours(source_img, vector <vector <Point>>(1, contour), -1, colour, 2);
    cv::Moments mu = cv::moments(contour);
    cv::Point centroid(mu.m10 / mu.m00, mu.m01 / mu.m00);
    cv::putText(source_img, contour_label, centroid, cv::FONT_HERSHEY_SIMPLEX, 0.5, colour, 2);
}
