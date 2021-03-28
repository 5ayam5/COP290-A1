#include <opencv2/opencv.hpp>
#include "homography.hpp"

using namespace cv;
using namespace std;

const int QUEUE_THRESH = 15;
const int BLUR_KERNEL_SIZE = 45;
const int DYNAMIC_THRESH = 6;

// return the next frame in the video in grayscale
Mat getNextFrame(VideoCapture &vid, float scale = 0)
{
	Mat frame;
	if (vid.read(frame))
	{
		if (scale != 0)
			resize(frame, frame, Size(), 1.0 / scale, 1.0 / scale, INTER_AREA);
		cvtColor(frame, frame, COLOR_BGR2GRAY);
		return frame;
	}
	return Mat();
}

// warp and crop the image
void warpAndCrop(Mat &frame, vector<Point2f> corners, vector<Point2f> cornersMap, float scale = 1)
{
	warpPerspective(frame, frame, computeHomography(corners, scale), getRect(cornersMap).size());
}

// assert and blur
void blur(Mat &frame, int k)
{
	assert(k & 1);
	GaussianBlur(frame, frame, Size(k, k), 0, 0);
}

// compute the queue density
Mat computeQueue(Mat &refFrame, Mat &currFrame)
{
	Mat queue;
	absdiff(refFrame, currFrame, queue);
	threshold(queue, queue, QUEUE_THRESH, 1, 0);
	return queue;
}

Mat computeDynamic(Mat &prevFrame, Mat &currFrame)
{
	Mat dynamic;
	absdiff(prevFrame, currFrame, dynamic);
	blur(dynamic, BLUR_KERNEL_SIZE);
	threshold(dynamic, dynamic, DYNAMIC_THRESH, 1, 0);
	return dynamic;
}
