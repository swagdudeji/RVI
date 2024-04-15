#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>


    using namespace std;
    using namespace cv;

    Mat pre_process_image_(Mat& source_img);
    void detect_face(Mat & img, CascadeClassifier & cascade, CascadeClassifier & nestedCascade, CascadeClassifier & smile_cascade, double scale);
    void draw_rectangle(Mat& source_img, Rect rectangle_, Scalar colour);
    vector <Rect> get_rectangles_by_cascade(Mat source_img, CascadeClassifier& cascade, double scale_factor, int min_neighbours, Size size);


    int main(int argc, const char** argv)
    {
        VideoCapture capture;
        Mat frame, image;
        CascadeClassifier face_cascade, eyes_cascade, smile_cascade;
        face_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");
        eyes_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
        smile_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_smile.xml");
        double scale = 1;

        capture.open("D:\\ZUA.mp4");
        int frame_width = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH)); 
        int frame_height = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT)); 
        Size frame_size(frame_width, frame_height);
        int frames_per_second = 10;

        VideoWriter oVideoWriter("D:\\output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));
        if (capture.isOpened())
        {
            cout << "Someone bring me up the lobster" << endl;
            while (1)
            {
                capture >> frame;
                if (frame.empty())
                    break;
                Mat frame1 = frame.clone();

                detect_face(frame1, face_cascade, eyes_cascade, smile_cascade, scale);
                oVideoWriter.write(frame1);
                char c = (char)waitKey(10);

                if (c == 27 || c == 'q' || c == 'Q')
                    break;
            }
            oVideoWriter.release();
            cout << "SHEEEEEEEEEEEEESH";
        }
        else
            cout << "Could not Open Video/Camera! ";
        return 0;
    }

    Mat pre_process_image_(Mat& source_img) {
        Mat gray, image_blurred, resized_img;
        cvtColor(source_img, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, image_blurred, Size(3, 3), 0);
        double fx = 1;
        resize(image_blurred, resized_img, Size(), fx, fx, INTER_LINEAR);
        equalizeHist(resized_img, resized_img);
        return resized_img;
    }

    void draw_rectangle(Mat& source_img, Rect rectangle_, Scalar colour) {
        rectangle(source_img, Point(rectangle_.x, rectangle_.y), Point(rectangle_.x + rectangle_.width, rectangle_.y + rectangle_.height), colour, 4);
    }

    vector <Rect> get_rectangles_by_cascade(Mat source_img, CascadeClassifier& cascade, double scale_factor, int min_neighbours, Size size) {
        vector <Rect> rectangles;
        cascade.detectMultiScale(source_img, rectangles, scale_factor, min_neighbours, 0 | CASCADE_SCALE_IMAGE, size);
        return rectangles;
    }

    void detect_face(Mat & source_img, CascadeClassifier & face_cascade, CascadeClassifier & eyes_cascade, CascadeClassifier & smile_cascade, double scale)
    {
        Mat pre_processed_img = pre_process_image_(source_img);
        vector<Rect> faces, eyes, smiles;
        faces = get_rectangles_by_cascade(pre_processed_img, face_cascade, 1.1, 2, Size(30, 30));
        eyes = get_rectangles_by_cascade(pre_processed_img, eyes_cascade, 1.1, 2, Size(30, 30));
        smiles = get_rectangles_by_cascade(pre_processed_img, smile_cascade, 1.8, 22, Size(30, 30));

        for (Rect rect : faces) {
            draw_rectangle(source_img, rect, Scalar(0, 255, 0));
        }
        for (Rect rect : eyes) {
            draw_rectangle(source_img, rect, Scalar(0, 0, 255));
        }
        for (Rect rect : smiles) {
            draw_rectangle(source_img, rect, Scalar(255, 0, 0));
        }

        imshow("Face Detection", source_img);
    }
