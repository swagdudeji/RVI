#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

void draw_krasota(Mat & source_img);
vector <Mat> split_image(Mat source_img);
Mat get_circular_mask(Mat source_img, Point center, int radius);
void get_masked_image(Mat source_img, Mat output_img, Mat mask);

int main()
{
    Mat img = Mat(400, 500, CV_8UC3, 0.0); 

    draw_krasota(img);
    imshow("Krasota", img);
    waitKey(0);
    destroyWindow("Krasota");

    vector <Mat> splitted_mats = split_image(img);
    for (int i = 0; i < splitted_mats.size(); i++) {
        imshow(to_string(i), splitted_mats[i]);
    }
    waitKey(0);
    destroyAllWindows();

    Mat masked_image(img.size(), img.type());
    get_masked_image(img, masked_image, get_circular_mask(img, Point(250, 200), 200));
    imshow("Mask", masked_image);
    waitKey(0);
    destroyWindow("Mask");

    return 0;
}

void draw_krasota(Mat & source_img) {
    int font = FONT_HERSHEY_PLAIN;
    double font_scale = 2;
    Scalar colour(0, 0, 255);
    string text = "KOMP`UTER SEYCHAS VZORVETS`A!";
    Point text_pos(100, 100);
    putText(source_img, text, text_pos, font, 1, colour);
    string img_size_text = "Image Size: " + to_string(source_img.cols) + " x " + to_string(source_img.rows);
    Point img_size_text_pos(10, 30);
    putText(source_img, img_size_text, img_size_text_pos, font, 2, colour);

    rectangle(source_img, Rect(110, 110, 100, 100), colour, 2);
    circle(source_img, Point(300, 100), 50, colour, 2);
    circle(source_img, Point(100, 300), 50, colour, -1);
    line(source_img, Point(500, 0), Point(300, 400), colour, 2);
}

vector <Mat> split_image(Mat source_img) {

    int x_mid = source_img.cols / 2;
    int y_mid = source_img.rows / 2;
    vector <Mat> processed_mats;

    for (int i = 0; i < source_img.cols; i += source_img.cols / 2) {
        for (int j = 0; j < source_img.rows; j += source_img.rows / 2) {
            processed_mats.push_back(source_img(Rect(i, j, x_mid, y_mid)).clone());
        }
    }
    return processed_mats;
}

Mat get_circular_mask( Mat source_img, Point center, int radius) {
    Mat mask(source_img.size(), source_img.type());
    mask.setTo(0);
    circle(mask, center, radius, Scalar(255, 255, 255), FILLED);
    return mask;
}

void get_masked_image(Mat source_img, Mat output_img, Mat mask){
    output_img.setTo(0);
    source_img.copyTo(output_img, mask); 
}