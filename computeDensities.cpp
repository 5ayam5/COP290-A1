#include <opencv2/opencv.hpp>
#include <iostream>
#include "homography.hpp"

using namespace cv;
using namespace std;

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

	string fileName = argv[1];

	VideoCapture video(fileName);

	if (video.isOpened() == false || fileName.substr(fileName.size() - 3, 3) != "mp4")
	{
		cout << "Could not load file or file not of mp4 format, please check the file and try again :(\n";
		return 0;
	}

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

	while (true)
	{
		Mat currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);
		Mat queue, dynamic;
		absdiff(refFrame, currFrame, queue);
		absdiff(prevFrame, currFrame, dynamic);
		imshow("test", dynamic);
		waitKey(1);
		prevFrame = currFrame;
	}

	return 0;
}
