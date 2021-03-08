#include <opencv2/opencv.hpp>
#include <iostream>
#include "homography.hpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	// check if file address present
	if (argc != 2)
	{
		cout << "Required one additional argument:\n./main <file name>\n";
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

	// compute homography matrix from the given corner points and store+display the resultant image
	Mat h = computeHomography(corners);
	warpPerspective(img, perspectiveImg, h, Size(img.rows, img.cols));
	imshow("Projected " + fileName, perspectiveImg);
	cout << "Press any key to view cropped image and save this projected image, esc to quit\n";
	int val = waitKey(0);
	if (val == 27)
		return 0;
	try
	{
		destroyWindow("Projected " + fileName);
		imwrite(fileName.substr(0, fileName.size() - 4) + "Projected.jpg", perspectiveImg);
	}
	catch (Exception &e)
	{
		return 0;
	}

	// compute the cropped version of projected image
	Mat croppedImg = perspectiveImg(getRect(findMap(corners)));
	imshow("Cropped " + fileName, croppedImg);
	cout << "Press any key to save image and terminate, esc to quit\n";
	val = waitKey(0);
	if (val == 27)
		return 0;
	imwrite(fileName.substr(0, fileName.size() - 4) + "Cropped.jpg", croppedImg);

	return 0;
}