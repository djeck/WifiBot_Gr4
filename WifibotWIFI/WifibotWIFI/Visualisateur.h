#ifndef VISUALISATEUR_HPP
#define VISUALISATEUR_HPP

#include <opencv2/opencv.hpp>
#include <iostream>

#define MAP_SIZE 800
#define MAP_ZOOM 0.3
#define ROBOT_RADIUS 4

class Visualisateur
{
public:
	Visualisateur();
	void setPosition(double x, double y);
	void drawPointOnMap();
	void display();

private:
	cv::Mat map;
	cv::Point pt;

};

#endif # VISUALISATEUR_HPP
