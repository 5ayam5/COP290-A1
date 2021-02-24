#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// add the clicked point to the points vector
void mouseHandler(int event, int x, int y, int, void *data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	vector<Point2f> *corners = (vector<Point2f> *)data;
	corners->push_back(Point2f(x, y));
	cout << "Clicked at " << x << ", " << y << "\n";
	if (corners->size() == 4)
		cout << "Press any key to obtain projected image\n";
}

// rearranges the points in order to remove a bowtie formed if any
/*
bool pSortX(Point2f &a,Point2f &b){
	return (a.x<b.x);
}
bool pSortY(Point2f &a,Point2f &b){
	return (a.y<b.y);
}*/

int area(vector<Point2f> &corners){
	return (corners[0].x*corners[1].y+corners[1].x*corners[2].y+corners[2].x*corners[3].y+corners[1].x*corners[0].y)-(corners[1].x*corners[0].y+corners[2].x*corners[1].y+corners[3].x*corners[2].y+corners[0].x*corners[3].y);
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
	if (area(corners)<area(Coord)){
		corners=Coord;
	}
}
// find the coordinates in the new perspective by taking the new lengths in rectangle to be mean of opposite sides(?)
vector<Point2f> findMap(vector<Point2f> corners)
{
	return vector<Point2f>({{472, 52}, {800, 52}, {800, 830}, {472, 830}});
}

// utility function to compute and return homography 
Mat computeHomography(vector<Point2f> &corners)
{
	reorderPoints(corners);
	return findHomography(corners, findMap(corners));
}

Rect getRect(vector<Point2f> points)
{
	if (points.size() < 3)
		return Rect();
	int x = points[0].x, y = points[0].y;
	return Rect(x, y, int(points[1].x - x), int(points[2].y - y));
}

int main(int argc, char *argv[])
{
	// check if file address present
	if (argc != 2)
	{
		cout << "Required one argument:\n./main <file name>\n";
		return 0;
	}

	string fileName = argv[1];

	// read the image and check if read correctly
	Mat img = imread(fileName, IMREAD_GRAYSCALE);
	if (img.data == NULL || fileName.substr(fileName.size() - 3, 3) != "jpg")
	{
		cout << "Could not load file or file not of jpeg format, please check the file and try again :(\n";
		return 0;
	}

	Mat perspectiveImg = Mat::zeros(img.rows, img.cols, img.type());
	string windowName = "Select corners: " + fileName;

	// wait for 4 mouse clicks and store the points in vector "corners"
	vector<Point2f> corners;
	cout << "Click on the four corners of the road\n";
	while (true)
	{
		imshow(windowName, img);
		setMouseCallback(windowName, mouseHandler, &corners);
		int val = waitKey(0);
		if (val == 27)
			return 0;
		if (corners.size() == 4)
		{
			destroyWindow(windowName);
			break;
		}
	}
	//testing
	cout<<corners<<endl;
    reorderPoints(corners);
	cout<<corners<<endl;

	// compute homography matrix from the given corner points and store+display the resultant image
	Mat h = computeHomography(corners);
	warpPerspective(img, perspectiveImg, h, Size(img.rows, img.cols));
	imwrite(fileName.substr(0, fileName.size() - 4) + "Projected.jpg", perspectiveImg);
	imshow("Projected " + fileName, perspectiveImg);
	cout << "Press any key to view cropped image\n";
	waitKey(0);
	destroyWindow("Projected " + fileName);

	// compute the cropped version of projected image
	Mat croppedImg = perspectiveImg(getRect(findMap(corners)));
	imwrite(fileName.substr(0, fileName.size() - 4) + "Cropped.jpg", croppedImg);
	imshow("Cropped " + fileName, croppedImg);
	cout << "Press any key to terminate\n";
	waitKey(0);
}