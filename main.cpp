#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void mouseHandler(int event, int x, int y, int, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	vector<Point2f> *corners = (vector<Point2f> *)data;
	corners->push_back(Point2f(x, y));
	cout << x << " " << y << "\n";
	if (corners->size() == 4)
		cout << "Press any key to continue\n";
}

void reorderPoints(vector<Point2f> &corners)
{

}

vector<Point2f> findMap(vector<Point2f> corners)
{
	return vector<Point2f>({{472, 52}, {800, 52}, {800, 830}, {472, 830}});
}

Mat computeHomography(vector<Point2f> &corners)
{
	reorderPoints(corners);
	return findHomography(corners, findMap(corners));
}

int main(int argc, char *argv[])
{
	// check if file address present
	if (argc != 2)
	{
		cout << "Required one additional argument:\n./main <file name>\n";
		return 0;
	}

	// read the image and check if read correctly
	Mat img = imread(argv[1], IMREAD_GRAYSCALE);
	if (img.data == NULL)
	{
		cout << "Could not load file, please check the file and try again :(\n";
		return 0;
	}

	Mat perspectiveImg = Mat::zeros(img.rows, img.cols, img.type());
	string windowName = "Select corners: " + string(argv[1]);

	vector<Point2f> corners;

	while (true)
	{
		imshow(windowName, img);
		setMouseCallback(windowName, mouseHandler, &corners);
		int val = waitKey(0);
		if (corners.size() == 4)
		{
			destroyWindow(windowName);
			break;
		}
	}

	Mat h = computeHomography(corners);

	warpPerspective(img, perspectiveImg, h, Size(img.rows, img.cols));
	cout << h.rows << "\n";
	imshow("works?", perspectiveImg);
	waitKey(0);
}