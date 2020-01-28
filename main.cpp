#include<iostream>
#include<opencv2/opencv.hpp>
#include<stdlib.h>
#include<time.h>
#define KERNEL_SIZE 22
using namespace std;
using namespace cv;
const char* imagePath = "d:/sample.bmp";
const char* winName = "My Window";

void main(void)
{
	assignment();
}

void assignment()
{
	VideoCapture camera(1);
	bool press = false;
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.5;
	int thickness = 1;
	int baseline = 0;
	int sum;
	int* r;
	int* blue;
	int* red;
	int* green;


	if (!camera.isOpened()) {
		cerr << "error\n";
		return;
	}

	namedWindow(" ", CV_WINDOW_NORMAL);
	resizeWindow(" ", 1024, 768);

	while (true) {
		String msg = "Coins : ";
		sum = 0;
		Mat frame;
		camera >> frame;
		if (frame.empty())
			break;

		imshow(" ", frame);

		char key = cvWaitKey(10);

		if (key == 27 && !press)
			break;
		else {
			press = true;
			if (key == 'a') {
				Mat showMat;

				/*cvtColor(frame, showMat, COLOR_BGR2GRAY);
				GaussianBlur(showMat, showMat, cv::Size(3, 3), 2.0, 2.0);
				double mask1[3][3] = { { 1.0, -2.0, 1.0 }, { -2.0, 5.0, -2.0 }, { 1.0, -2.0, 1.0 } };
				Mat kernel1(3, 3, CV_64F, mask1);
				filter2D(showMat, showMat, showMat.depth(), kernel1);*/
				// 흰색 용지용.

				cvtColor(frame, showMat, COLOR_BGR2HSV);
				vector<Mat> channels;
				split(showMat, channels);
				Mat sat = channels[1];
				showMat = sat.clone();

				GaussianBlur(showMat, showMat, cv::Size(3, 3), 2.0, 2.0);
				double mask1[3][3] = { { 1.0, -2.0, 1.0 }, { -2.0, 5.0, -2.0 }, { 1.0, -2.0, 1.0 } };
				Mat kernel1(3, 3, CV_64F, mask1);
				filter2D(showMat, showMat, showMat.depth(), kernel1);

				erode(showMat, showMat, kernel1, Point(-1, -1), 1);
				dilate(showMat, showMat, kernel1, Point(-1, -1), 1);
				erode(showMat, showMat, kernel1, Point(-1, -1), 1);
				dilate(showMat, showMat, kernel1, Point(-1, -1), 1);
				//namedWindow("asdf", CV_WINDOW_NORMAL);
				//imshow("asdf", sat)

				// 신문지 용지용.

				float *rArr, *gArr, *bArr;

				vector<Vec3f> circles;
				HoughCircles(showMat, circles, HOUGH_GRADIENT, 1, showMat.rows / 16, 50, 25, 18, 45);
				// 흰색 종이, 신문지 전용.

				int size = circles.size();
				r = new int[size];
				red = new int[size];
				green = new int[size];
				blue = new int[size];

				rArr = new float[size];
				gArr = new float[size];
				bArr = new float[size];
				int bSum, gSum, rSum;

				int rMax = 0, rMin = 100;
				float rRGBMax = 0, rRGBMin = 1, gRGBMax = 0, gRGBMin = 1, bRGBMax = 0, bRGBMin = 1;

				for (size_t i = 0; i < circles.size(); i++) {
					Vec3i c = circles[i];
					Point center = Point(c[0], c[1]);
					circle(frame, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
					int radius = c[2];
					circle(frame, center, radius, Scalar(255, 0, 255), 3, LINE_AA);

					printf("circle[%2d] : (cx, cy) = (%d, %d), r = %.2f\n", i, c[0], c[1], float(c[2]));
					putText(frame, to_string(i), Point(c[0], c[1]), fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
					Vec3b color;
					bSum = 0;
					gSum = 0;
					rSum = 0;
					for (int q = 0; q < KERNEL_SIZE; q++) {
						for (int w = 0; w < KERNEL_SIZE; w++) {
							color = frame.at<Vec3b>(Point(c[0] - q, c[1] - w));
							bSum += color[0];
							gSum += color[1];
							rSum += color[2];
						}
					}

					red[i] = rSum / 400;
					green[i] = gSum / 400;
					blue[i] = bSum / 400;

					//float y = 0.3 * red + 0.59 * green + 0.1 * blue[i];
					//float u = (blue - y) * 0.493;
					//float v = (red - y) * 0.877;

					r[i] = c[2];

					int RGB = red[i] + green[i] + blue[i];
					rArr[i] = (float)red[i] / RGB;
					gArr[i] = (float)green[i] / RGB;
					bArr[i] = (float)blue[i] / RGB;

					if (rMax < r[i]) rMax = r[i];
					if (rMin > r[i]) rMin = r[i];
					if (rRGBMax < rArr[i]) rRGBMax = rArr[i];
					if (rRGBMin > rArr[i]) rRGBMin = rArr[i];
					if (gRGBMax < gArr[i]) gRGBMax = gArr[i];
					if (gRGBMin > gArr[i]) gRGBMin = gArr[i];
					if (bRGBMax < bArr[i]) bRGBMax = bArr[i];
					if (bRGBMin > bArr[i]) bRGBMin = bArr[i];

					printf("R: %d, G: %d, B: %d, R/RGB: %f, G/RGB: %f, B/RGB: %f R/G: %f G/B: %f R/B: %f\n", red[i], green[i], blue[i], rArr[i], gArr[i], bArr[i], rArr[i] / gArr[i], gArr[i] / bArr[i], rArr[i] / bArr[i]);

					//printf("Y : %.2f, U : %.2f, V : %.2f\n", y, u, v);

				}
				printf("\nrMax: %d, rMin: %d \nrRGBMax: %f, rRGBMin: %f \ngRGBMax: %f, gRGBMin : %f \nbRGBMax : %f, bRGBMin : %f\n\n",
					rMax, rMin, rRGBMax, rRGBMin, gRGBMax, gRGBMin, bRGBMax, bRGBMin);

				for (int i = 0; i < circles.size(); i++) {
					int num;
					if (r[i] >= 34) {
						num = 500;
						sum += 500;
					}
					else if (r[i] <= 27) {
						num = 10;
						sum += 10;
					}
					else if (rArr[i] / bArr[i] >= 1.5) {
						num = 10;
						sum += 10;
					}
					else {
						if (r[i] < 34 && r[i] >= 31) {
							num = 100;
							sum += 100;
						}
						else {
							num = 50;
							sum += 50;
						}
					}
					printf("%d번째 Circle : %d원\n", i, num);
				}
				printf("\n");

				msg += to_string(sum);
				putText(frame, msg, Point(10, 20), fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
				cout << endl;

				namedWindow("Result", CV_WINDOW_NORMAL);
				resizeWindow("Result", 1024, 768);
				imshow("Result", frame);


				delete[] r;
				delete[] red;
				delete[] green;
				delete[] blue;

				delete[] rArr;
				delete[] bArr;
				delete[] gArr;

				press = false;
			}
		}
	}
}
