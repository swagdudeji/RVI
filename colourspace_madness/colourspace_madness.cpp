#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

vector <Mat> colourspace_madness(Mat source_img, vector <int> colourspaces);

int main() {
    string img_path;
    cout << "Enter image path: "; cin >> img_path;

    Mat img = imread(img_path);

    if (img.empty()) {
        cout << "Error" << endl;
        return -1;
    }
    imshow("Bylo", img);
    waitKey(0);

    vector <int> colourspaces{ COLOR_BGR2GRAY, COLOR_BGR2HSV, COLOR_BGR2Lab, COLOR_BGR2YCrCb, COLOR_BGR2XYZ, COLOR_BGR2HLS, COLOR_BGR2Luv };
    vector <Mat> colourspace_mats = colourspace_madness(img, colourspaces);
    for (int i = 0; i < colourspaces.size(); i++) {
        imshow("Stalo " + to_string(colourspaces[i]), colourspace_mats[i]);
        waitKey(0);
    }
    //waitKey(0);

    return 0;
}

vector <Mat> colourspace_madness(Mat source_img, vector <int> colourspaces) {
    vector <Mat> colourspace_mats;
    Mat tmp_mat;
    for (int colourspace : colourspaces) {
        cvtColor(source_img, tmp_mat, colourspace);
        colourspace_mats.push_back(tmp_mat.clone());
    }
    return colourspace_mats;
}