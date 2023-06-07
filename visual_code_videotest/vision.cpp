
#include "opencv2/opencv.hpp"
#include <iostream>
#include "vision.hpp"

using namespace std;
using namespace cv;

Mat ROI(Mat frame) {
    //roi ����
    Mat roi = frame(Rect(Point(0, frame.rows * 3 / 4), Point(frame.cols, frame.rows)));
    // �׷��̽����� ��ȯ
    Mat gray;
    cvtColor(roi, gray, cv::COLOR_BGR2GRAY);
    //��հ� ���
    Scalar meanvalue = mean(gray);
    //��� ���� �ȼ� ��
    Mat pixel_val = gray + (100 - meanvalue[0]);
    //����ȭ
    Mat thr;//220
    threshold(pixel_val, thr, 170, 255, THRESH_BINARY);
    return thr;
}

double distance(Point2f p1, Point2f p2) { //�� �� ������ �Ÿ�
    //cout << abs(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y))) << endl;
    return abs(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));
}
void robot_control(double err) {


}
Mat line_detect(Mat frame, Point2f* prevLineCenter, Point2f* error_center) {
    Mat cvt, labels, stats, centroids;
    double error;
    int cnt = connectedComponentsWithStats(frame, labels, stats, centroids);
    cvtColor(frame, cvt, COLOR_GRAY2BGR);

    // ���� ��ü ���� , ������ ����
    int lineLabel = -1;
    int minArea = 500;  // �ּ� ���� �Ӱ谪
    int maxArea = 10000; // �ִ� ���� �Ӱ谪
    Point2f line_center;
    int max = 0, min = 0;
    vector<double> len;
    vector<Point> len_p;
    if (cnt > 1) {
        cout << "����" << cnt << endl;
        for (int i = 1; i < cnt; i++)
        {
            int* ptr = stats.ptr<int>(i);
            //Point2f pt = Point2f(centroids.at<double>(i, 0), centroids.at<double>(i, 1)); // ������ �����߽�

            if (i == 1) { max = ptr[4]; }
            if (max <= ptr[4] && ptr[4] >= minArea) {
                cout << "����" << ptr[4] << endl;
                //*prevLineCenter = line_center;
                max = ptr[4];
                lineLabel = i;
                //line_center = Point(centroids.at<double>(lineLabel, 0), centroids.at<double>(lineLabel, 1));
                //*prevLineCenter = line_center;
            }
        }
        double* d = centroids.ptr<double>(lineLabel);
        line_center = Point2d(d[0], d[1]);

        // if (distance(line_center, *prevLineCenter) > 50) {
        //     line_center = *prevLineCenter;
        // }
        if (abs((*prevLineCenter).x - line_center.x) > 150) {
            line_center = *prevLineCenter;
        }
    }
    else {
        line_center = *prevLineCenter;
        //lineLabel = *preLineLabel;
        cout << "xxxxx" << endl;
    }
    *prevLineCenter = line_center;
    for (int i = 1; i < cnt; i++)
    {

        if (i == lineLabel)
        {

            int* ptr = stats.ptr<int>(lineLabel);

            rectangle(cvt, Rect(ptr[0], ptr[1], ptr[2], ptr[3]), Scalar(0, 0, 255));
            circle(cvt, Point(centroids.at<double>(i, 0), centroids.at<double>(i, 1)), 3, Scalar(0, 0, 255), -1);

        }
        else
        {
            int* ptr = stats.ptr<int>(i);
            rectangle(cvt, Rect(ptr[0], ptr[1], ptr[2], ptr[3]), Scalar(255, 0, 0));
            circle(cvt, Point(centroids.at<double>(i, 0), centroids.at<double>(i, 1)), 3, Scalar(255, 0, 0), -1);
        }

    }
    *error_center = line_center;
    return cvt;
}

double get_error(Mat frame, Point2f line_center) {
    double error = (frame.cols / 2) - line_center.x;
    cout << "error:" << error << endl;
    return error;
}