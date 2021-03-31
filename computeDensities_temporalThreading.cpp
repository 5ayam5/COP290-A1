#include <opencv2/opencv.hpp>
#include <iostream>
#include "densities.hpp"

using namespace cv;
using namespace std;

struct ThreadArgs
{
	static int NUM_THREADS;
	static Mat refFrame;
	static vector<double> qVals;
	static vector<Point2f> corners, cornersMap;
	Mat frame;
	int thread_id, frameNum;
};
int ThreadArgs::NUM_THREADS;
Mat ThreadArgs::refFrame;
vector<Point2f> ThreadArgs::corners, ThreadArgs::cornersMap;
vector<double> ThreadArgs::qVals;

void *computeQueueThread(void *arg)
{
	ThreadArgs *threadArgs = (ThreadArgs *)arg;
	warpAndCrop(threadArgs->frame, ThreadArgs::corners, ThreadArgs::cornersMap);
	Mat queue = computeQueue(ThreadArgs::refFrame, threadArgs->frame);
	ThreadArgs::qVals[threadArgs->frameNum] = sum(queue)[0] * 1.0 / (queue.rows * queue.cols);

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
	ThreadArgs::refFrame = getNextFrame(video);
	if (ThreadArgs::refFrame.data == NULL)
	{
		cerr << "Error in reading the video file, please check the file and try again.\n";
		return 0;
	}
	ThreadArgs::corners = vector<Point2f>({{998, 222}, {1268, 223}, {1520, 1015}, {384, 1015}});
	ThreadArgs::cornersMap = findMap(ThreadArgs::corners);
	cerr << "Press enter key\n";
	imshow("", ThreadArgs::refFrame);
	waitKey(0);
	destroyAllWindows();
	warpAndCrop(ThreadArgs::refFrame, ThreadArgs::corners, ThreadArgs::cornersMap);
	int frameNum = 0, totFrames = video.get(CAP_PROP_FRAME_COUNT);
	ThreadArgs::qVals.resize(totFrames - 1);
	ThreadArgs::NUM_THREADS = stoi(argv[2]);
	auto t = chrono::high_resolution_clock::now();

	pthread_t threads[ThreadArgs::NUM_THREADS];
	vector<ThreadArgs> threadArgs(ThreadArgs::NUM_THREADS);
	for (int i = 0; i < ThreadArgs::NUM_THREADS; ++i)
		threadArgs[i].thread_id = i;

	cout << "time (in seconds),queue density\n";

	// loop for the complete video
	while (frameNum < totFrames - 1)
	{
		for (int i = 0; i < min(totFrames - frameNum - 1, ThreadArgs::NUM_THREADS); ++i)
		{
			threadArgs[i].frame = getNextFrame(video);
			threadArgs[i].frameNum = frameNum + i;
			int rc = pthread_create(&threads[i], NULL, computeQueueThread, (void *)&threadArgs[i]);
			if (rc)
			{
				cerr << "Unable to create thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}
		for (int i = 0; i < min(totFrames - frameNum - 1, ThreadArgs::NUM_THREADS); ++i)
		{
			int rc = pthread_join(threads[i], NULL);
			if (rc)
			{
				cerr << "Unable to join thread " << i << ", frame number: " << frameNum << '\n';
				return 0;
			}
		}

		frameNum += ThreadArgs::NUM_THREADS;
	}

	for (int i = 0; i < totFrames - 1; ++i)
		cout << (i + 1) * 1.0 / fps << ',' << ThreadArgs::qVals[i] << '\n';

	cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t).count() / 1000.0;

	return 0;
}
