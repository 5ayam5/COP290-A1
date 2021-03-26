#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>
#include "densities.hpp"

using namespace cv;
using namespace std;

// segment the frame so that each segment is processed by a different thread
void segmentFrame(int x, Mat &frame, vector<Mat> &segmentedFrame)
{
	int width = frame.size().width, height = frame.size().height / x;
	for (int i = 0; i < x - 1; ++i)
		segmentedFrame[i] = frame(Rect(0, i * height, width, height));
	segmentedFrame.back() = frame(Rect(0, (x - 1) * height, width, frame.size().height - (x - 1) * height));
}

vector<double> qVals;
struct ThreadArgs
{
	int thread_id;
	Mat *frame;
	Mat *refFrame;
};

// function to compute queue density for each thread
void *computeQueueThread(void *arg)
{
	ThreadArgs *threadArgs = (ThreadArgs *)arg;

	Mat queue = computeQueue(*(threadArgs->refFrame), *(threadArgs->frame));
	qVals[threadArgs->thread_id] = sum(queue)[0] * 1.0 / (queue.rows * queue.cols);

	pthread_exit(NULL);
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
	int frameNum = 0, x;

	cerr << "Number of threads: ";
	cin >> x;
	auto t = chrono::high_resolution_clock::now();

	pthread_t threads[x];
	vector<Mat> segmentedRefFrame(x), segmentedCurrFrame(x);
	qVals.resize(x);
	vector<ThreadArgs> threadArgs(x);
	for (int i = 0; i < x; ++i)
	{
		threadArgs[i].refFrame = &segmentedRefFrame[i];
		threadArgs[i].frame = &segmentedCurrFrame[i];
		threadArgs[i].thread_id = i;
	}
	segmentFrame(x, refFrame, segmentedRefFrame);

	cout << "time (in seconds),queue density\n";

	while (true)
	{
		Mat currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);
		segmentFrame(x, currFrame, segmentedCurrFrame);

		for (int i = 0; i < x; ++i)
		{
			int rc = pthread_create(&threads[i], NULL, computeQueueThread, (void *)&threadArgs[i]);
			if (rc)
			{
				cerr << "Unable to create thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}

		for (int i = 0; i < x; ++i)
		{
			int rc = pthread_join(threads[i], NULL);
			if (rc)
			{
				cerr << "Unable to join thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}

		cout << ++frameNum * 1.0 / fps << ',' << accumulate(qVals.begin(), qVals.end(), (double)0) / x << '\n';
	}

	cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count() / 1000.0;

	return 0;
}
