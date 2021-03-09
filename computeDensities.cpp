#include <opencv2/opencv.hpp>
#include <iostream>
#include "homography.hpp"

using namespace cv;
using namespace std;

const QUEUE_THRESH = 30;

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
	// check if file address present
	if (argc != 2)
	{
		cout << "Required one additional argument:\n./main <file name>\n";
		return 0;
	}

	// attempt to open video
	string fileName = argv[1];
	VideoCapture video(fileName);
	if (video.isOpened() == false || fileName.substr(fileName.size() - 3, 3) != "mp4")
	{
		cout << "Could not load file or file not of mp4 format, please check the file and try again :(\n";
		return 0;
	}

	// initialise variables and read the first frame
	Mat refFrame = getNextFrame(video);
	if (refFrame.data == NULL)
	{
		cout << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}
	vector<Point2f> corners = getCorners(refFrame, "Select corners");
	if (corners.size() != 4)
	{
		cout << "Escape key pressed, aborting execution\n";
		return 0;
	}
	vector<Point2f> cornersMap = findMap(corners);
	warpAndCrop(refFrame, corners, cornersMap);
	Mat prevFrame = refFrame;
	int n, frameNum = 0;
	cin >> n;
	double M = 0;

	// loop for the complete video
	while (true)
	{
		Mat currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);
		Mat queue, dynamic;
		absdiff(refFrame, currFrame, queue);
		absdiff(prevFrame, currFrame, dynamic);
		threshold(queue, queue, QUEUE_THRESH, 1, 0);
		// (sum(queue))[0] * 1.0 / (queue.rows * queue.cols)
		threshold(dynamic, dynamic, n, 255, 0);
		M = max(M, (sum(dynamic))[0] * 1.0 / (queue.rows * queue.cols * 255));
		cout << (sum(dynamic))[0] * 1.0 / (queue.rows * queue.cols * 255) << '\n';
		imshow("", dynamic);
		waitKey(1);
		prevFrame = currFrame;
	}
	cout << "M: " << M << '\n';

	return 0;
}
