#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// add the clicked point to the points vector
void mouseHandler(int event, int x, int y, int, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	vector<Point2f> *corners = (vector<Point2f> *)data;
	corners->push_back(Point2f(x, y));
	cerr << "Clicked at " << x << ", " << y << "\n";
	if (corners->size() == 4)
		cerr << "Press any key to obtain projected image\n";
}

int area(vector<Point2f> &corners)
{
	return (corners[0].x * corners[1].y + corners[1].x * corners[2].y + corners[2].x * corners[3].y + corners[1].x * corners[0].y) - (corners[1].x * corners[0].y + corners[2].x * corners[1].y + corners[3].x * corners[2].y + corners[0].x * corners[3].y);
}
// rearranges the points in cyclic order starting from the top left and moving clockwise
void reorderPoints(vector<Point2f> &corners)
{
	vector<Point2f> Coord;

	for (int i = 0; i < 2; i++)
	{
		Coord.push_back(corners[i]);
	}
	Coord.push_back(corners[3]);
	Coord.push_back(corners[2]);
	if (area(corners) < area(Coord))
	{
		corners = Coord;
	}
}

// find the coordinates in the new perspective by taking the new lengths in rectangle to be mean of opposite sides(?)
vector<Point2f> findMap(vector<Point2f> &corners, float scale = 1)
{
	return vector<Point2f>({{472 / scale, 52 / scale}, {800 / scale, 52 / scale}, {800 / scale, 830 / scale}, {472 / scale, 830 / scale}});
}

// utility function to compute and return homography
Mat computeHomography(vector<Point2f> &corners, float scale = 1)
{
	reorderPoints(corners);
	return findHomography(corners, findMap(corners, scale));
}

Rect getRect(vector<Point2f> points)
{
	if (points.size() != 4)
		return Rect();
	int x = points[0].x, y = points[0].y;
	return Rect(x, y, int(points[1].x - x), int(points[2].y - y));
}

// wait for 4 mouse clicks and store the points in vector "corners"
vector<Point2f> getCorners(Mat &frame, string windowName)
{
	vector<Point2f> corners;
	cerr << "Click on the four corners of the road\n";
	while (true)
	{
		imshow(windowName, frame);
		setMouseCallback(windowName, mouseHandler, &corners);
		int val = waitKey(0);
		if (val == 27)
			return vector<Point2f>();
		if (corners.size() == 4)
		{
			destroyWindow(windowName);
			break;
		}
	}
	return corners;
}
