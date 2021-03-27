#include <opencv2/opencv.hpp>
#include <iostream>
#include "densities.hpp"

using namespace cv;
using namespace std;

const int QUEUE = 6;

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
	Mat refFrameI = getNextFrame(video);
	if (refFrameI.data == NULL)
	{
		cerr << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}
	vector<Point2f> corners = getCorners(refFrameI, "Select corners");
	if (corners.size() != 4)
	{
		cerr << "Escape key pressed, aborting execution\n";
		return 0;
	}


	vector<Point2f> cornersMap = findMap(corners);
	warpAndCrop(refFrameI, corners, cornersMap);

		//resize refFrame
	Mat refFrame;
	resize(refFrameI, refFrame, Size(640, 480), 0, 0, INTER_AREA);
	


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
		Mat currFrameI = getNextFrame(video);
		if (currFrameI.data == NULL)
			break;
		warpAndCrop(currFrameI, corners, cornersMap);

//resize currFrame
		Mat currFrame;
		resize(currFrameI, currFrame, Size(640, 480), 0, 0, INTER_AREA);		


		Mat queue = computeQueue(refFrame, currFrame), dynamic = computeDynamic(prevFrame, currFrame);

		double currSum = sum(dynamic)[0] * 1.0 / (queue.rows * queue.cols);
		movingSum += currSum - prevSums.front();
		double qVal = sum(queue)[0] * 1.0 / (queue.rows * queue.cols), dVal = movingSum / QUEUE;
		cout << frameNum * 1.0 / fps << ',' << qVal << ',' << dVal << '\n';

		prevFrame = currFrame;
		prevSums.push(currSum);
		prevSums.pop();


		++frameNum;
	}

	return 0;
}
