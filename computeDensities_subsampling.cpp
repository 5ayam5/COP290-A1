#include <opencv2/opencv.hpp>
#include <iostream>
#include "densities.hpp"

using namespace cv;
using namespace std;

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
	int frameNum = 0, totFrames = video.get(CAP_PROP_FRAME_COUNT), x;

	cerr << "Subsampling parameter: ";
	cin >> x;
	auto t = chrono::high_resolution_clock::now();

	cout << "time (in seconds),queue density\n";

	// loop for the complete video
	while (frameNum < totFrames - 1)
	{
		Mat currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);

		Mat queue = computeQueue(refFrame, currFrame);

		double qVal = sum(queue)[0] * 1.0 / (queue.rows * queue.cols);
		for (int idx = frameNum; idx < min(totFrames - 1, frameNum + x); ++idx)
			cout << (idx + 1) * 1.0 / fps << ',' << qVal << '\n';

		for (int i = 1; i < x; ++i)
			++frameNum, getNextFrame(video);
		++frameNum;
	}

	cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count() / 1000.0;

	return 0;
}
