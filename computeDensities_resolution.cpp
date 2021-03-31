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
	if (argc != 3)
	{
		cerr << "Required two additional argument:\n./<program name> <file name> <parameter>\n";
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
	int frameNum = 1;
	float x = stof(argv[2]);

	// initialise variables and read the first frame
	Mat refFrame = getNextFrame(video, x);
	if (refFrame.data == NULL)
	{
		cerr << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}

	vector<Point2f> corners({{998 / x, 222 / x}, {1268 / x, 223 / x}, {1520 / x, 1015 / x}, {384 / x, 1015 / x}});
	vector<Point2f> cornersMap = findMap(corners, x);
	cerr << "Press enter key\n";
	imshow("", refFrame);
	waitKey(0);
	destroyAllWindows();
	warpAndCrop(refFrame, corners, cornersMap, x);

	auto t = chrono::high_resolution_clock::now();
	cout << "time (in seconds),queue density\n";

	// loop for the complete video
	while (true)
	{
		Mat currFrame = getNextFrame(video, x);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap, x);

		Mat queue = computeQueue(refFrame, currFrame);
		double qVal = sum(queue)[0] * 1.0 / (queue.rows * queue.cols);
		cout << frameNum * 1.0 / fps << ',' << qVal << '\n';

		++frameNum;
	}

	cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count() / 1000.0;

	return 0;
}
