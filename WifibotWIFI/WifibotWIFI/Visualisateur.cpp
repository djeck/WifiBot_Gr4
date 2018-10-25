#include "Visualisateur.h"

Visualisateur::Visualisateur()
{
	map = cv::Mat::zeros(MAP_SIZE, MAP_SIZE, CV_8UC3);
	pt = cv::Point(0, 0);
}

void Visualisateur::setPosition(double x, double y)
{
	pt.x = (unsigned int)(x*MAP_ZOOM);
	pt.y = (unsigned int)(y*MAP_ZOOM);
	cv::namedWindow("Map", cv::WINDOW_AUTOSIZE);
}

void Visualisateur::drawPointOnMap()
{
	circle(map, pt, ROBOT_RADIUS, cv::Scalar(0, 0, 255), -1, 8);
}

void Visualisateur::display()
{
	cv::imshow("Map", map);
}
