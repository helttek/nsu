#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <random>
using namespace cv;
using namespace std;

int main() {
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        cout << "Error opening video stream" << endl;
        return -1;
    }

    chrono::high_resolution_clock::time_point finish4, finish1, finish2, finish3;
    int i = 0, inputDurationSum = 0, processingDurationTime = 0, outputDurationTime = 0;
    chrono::high_resolution_clock::time_point start4 = chrono::high_resolution_clock::now();
    while (true) {
        Mat frame;
        // finish1 = chrono::high_resolution_clock::now();
        capture >> frame;
        if (frame.empty()) {
            finish4 = chrono::high_resolution_clock::now();
            break;
        }
        ++i;

        // chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
        Mat newFrame = frame;

        double angle = 15;
        // get the center coordinates of the image to create the 2D rotation matrix
        Point2f center((newFrame.cols - 1) / 2.0, (newFrame.rows - 1) / 2.0);
        Mat rotation_matix = getRotationMatrix2D(center, angle, 1.0);
        Mat rotated_image;
        warpAffine(newFrame, rotated_image, rotation_matix, newFrame.size());

        Mat imageLine = rotated_image.clone();

        Mat kernel2 = Mat::ones(5, 5, CV_64F); //magic
        kernel2 = kernel2 / 25; //magic
        filter2D(imageLine, imageLine, -1, kernel2, Point(-1, -1), 0, 4);
        putText(imageLine, "some text!", Point(50, 350), FONT_HERSHEY_COMPLEX, 1.5, Scalar(250, 225, 100));

        // chrono::high_resolution_clock::time_point finish = chrono::high_resolution_clock::now();
        // chrono::duration<double> timeSpan = chrono::duration_cast<chrono::duration<double>>(finish - start);
        // cout << "The frame was: " << timeSpan.count() << " seconds" << endl;

        imshow("Frame", imageLine);
        // imshow("Original", frame);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27) {
            finish4 = chrono::high_resolution_clock::now();
            break;
        }
    }chrono::duration<double> timeSpan = chrono::duration_cast<chrono::duration<double>>(finish4 - start4);
    cout << "The program processes " << i / timeSpan.count() << " frames in a second\n";

    capture.release();
    destroyAllWindows();
    return 0;
}
