#include "WifibotClient.h"

//#pragma once 

#define FLAG_BACKWARD_DEFAULT 128+32+1
#define FLAG_FORWARD_DEFAULT 128+32+64+16+1
#define FLAG_RIGHT_DEFAULT 128+32+64+1
#define FLAG_LEFT_DEFAULT 128+32+16+1
#define FLAG_STOP_DEFAULT 64+16+1
#define UINT8_t unsigned char 
#define IP_ADRESSE "192.168.1.77"
#define PORT	15020

#define THRESH_RIGHT 30 // seuils infrarouge pour la detection d'obstacle
#define THRESH_LEFT 30

class RobotLibrary {
public: 
	RobotLibrary();
	void updateSensorData();
	void move(int right, int left);
	bool obstableDroite();
	bool obstacleGauche();
	void forward();
	void backward();
	void left();
	void right();
	void stop();
	SensorData* getData();
	void processOdometry();
private: 
	WifibotClient robot;
	SensorData sensors_data;
	double x, y;
};