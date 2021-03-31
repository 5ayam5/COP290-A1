#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>
#include "densities.hpp"

using namespace cv;
using namespace std;

struct ThreadArgs
{
	static int NUM_THREADS;
	static Mat *frame;
	int thread_id;
	double qVal;
	Mat refFrame;
};
int ThreadArgs::NUM_THREADS;
Mat *ThreadArgs::frame;

// segment the frame so that each segment is processed by a different thread
Mat getSegment(Mat &frame, int i)
{
	Mat ret;
	int width = frame.size().width, height = frame.size().height / ThreadArgs::NUM_THREADS;
	if (i < ThreadArgs::NUM_THREADS - 1)
		ret = frame(Rect(0, i * height, width, height));
	else
		ret = frame(Rect(0, (ThreadArgs::NUM_THREADS - 1) * height, width, frame.size().height - (ThreadArgs::NUM_THREADS - 1) * height));
	return ret;
}

// function to compute queue density for each thread
void *computeQueueThread(void *arg)
{
	ThreadArgs *threadArgs = (ThreadArgs *)arg;

	int i = threadArgs->thread_id;
	Mat frame = getSegment(*(threadArgs->frame), i), queue = computeQueue(threadArgs->refFrame, frame);
	threadArgs->qVal = sum(queue)[0] * 1.0;

	pthread_exit(NULL);
}

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

	// initialise variables and read the first frame
	Mat refFrame = getNextFrame(video), currFrame;
	if (refFrame.data == NULL)
	{
		cerr << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}
	vector<Point2f> corners({{998, 222}, {1268, 223}, {1520, 1015}, {384, 1015}});
	vector<Point2f> cornersMap = findMap(corners);
	cerr << "Press enter key\n";
	imshow("", refFrame);
	waitKey(0);
	destroyAllWindows();
	warpAndCrop(refFrame, corners, cornersMap);
	int frameNum = 0;
	ThreadArgs::NUM_THREADS = stoi(argv[2]);
	auto t = chrono::high_resolution_clock::now();

	pthread_t threads[ThreadArgs::NUM_THREADS];
	vector<ThreadArgs> threadArgs(ThreadArgs::NUM_THREADS);
	for (int i = 0; i < ThreadArgs::NUM_THREADS; ++i)
	{
		threadArgs[i].refFrame = getSegment(refFrame, i);
		threadArgs[i].thread_id = i;
	}
	ThreadArgs::frame = &currFrame;

	cout << "time (in seconds),queue density\n";

	while (true)
	{
		currFrame = getNextFrame(video);
		if (currFrame.data == NULL)
			break;
		warpAndCrop(currFrame, corners, cornersMap);

		for (int i = 0; i < ThreadArgs::NUM_THREADS; ++i)
		{
			int rc = pthread_create(&threads[i], NULL, computeQueueThread, (void *)&threadArgs[i]);
			if (rc)
			{
				cerr << "Unable to create thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}

		double qVal = 0;
		for (int i = 0; i < ThreadArgs::NUM_THREADS; ++i)
		{
			int rc = pthread_join(threads[i], NULL);
			qVal += threadArgs[i].qVal;
			if (rc)
			{
				cerr << "Unable to join thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}
		cout << ++frameNum * 1.0 / fps << ',' << qVal / (currFrame.size().width * currFrame.size().height) << '\n';
	}

	cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count() / 1000.0;

	return 0;
}
