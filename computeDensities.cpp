#include <opencv2/opencv.hpp>
#include <iostream>
#include "homography.hpp"

using namespace cv;
using namespace std;

const int QUEUE_THRESH = 15;
const int BLUR_KERNEL_SIZE = 45;
const int DYNAMIC_THRESH = 6;
const int QUEUE = 6;

void blur(Mat &frame, int k)
{
	assert(k & 1);
	GaussianBlur(frame, frame, Size(k, k), 0, 0);
}

Mat getNextFrame(VideoCapture &vid)
{
	Mat frame;
	if (vid.read(frame))
	{
		cvtColor(frame, frame, COLOR_BGR2GRAY);
		return frame;
	}
	return Mat();
}

void warpAndCrop(Mat &frame, vector<Point2f> corners, vector<Point2f> cornersMap)
{
	warpPerspective(frame, frame, computeHomography(corners), Size(frame.rows, frame.cols));
	frame = frame(getRect(cornersMap));
}

int main(int argc, char *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);

	// check if file address present
	if (argc != 2)
	{
		cerr << "Required one additional argument:\n./main <file name>\n";
		return 0;
	}

	// attempt to open video
	string fileName = argv[1];
	VideoCapture video(fileName);
	if (video.isOpened() == false || fileName.substr(fileName.size() - 3, 3) != "mp4")
	{
		cerr << "Could not load file or file not of mp4 format, please check the file and try again :(\n";
		return 0;
	}
	double fps = video.get(CAP_PROP_FPS);

	// initialise variables and read the first frame
	Mat refFrame = getNextFrame(video);
	if (refFrame.data == NULL)
	{
		cerr << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}
	vector<Point2f> corners = getCorners(refFrame, "Select corners");
	if (corners.size() != 4)
	{
		cerr << "Escape key pressed, aborting execution\n";
		return 0;
	}
	vector<Point2f> cornersMap = findMap(corners);
	warpAndCrop(refFrame, corners, cornersMap);
	Mat prevFrame = refFrame;
	int frameNum = 1;
	double movingSum = 0;
	queue<double> prevSums;
	for (int i = 0; i < QUEUE; ++i)
		prevSums.push(0);

	cout << "time (in seconds),queue density,dynamic density\n";

	// loop for the complete video
	while (true)
	{
		Mat currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);

		Mat queue, dynamic;

		absdiff(refFrame, currFrame, queue);
		threshold(queue, queue, QUEUE_THRESH, 1, 0);

		absdiff(prevFrame, currFrame, dynamic);
		blur(dynamic, BLUR_KERNEL_SIZE);
		threshold(dynamic, dynamic, DYNAMIC_THRESH, 1, 0);

		double currSum = (sum(dynamic))[0] * 1.0 / (queue.rows * queue.cols);
		movingSum += currSum - prevSums.front();
		double qVal = (sum(queue))[0] * 1.0 / (queue.rows * queue.cols), dVal = movingSum / QUEUE;
		cout << frameNum * 1.0 / fps << ',' << qVal << ',' << dVal << '\n';

		prevFrame = currFrame;
		prevSums.push(currSum);
		prevSums.pop();


		++frameNum;
	}

	return 0;
}
