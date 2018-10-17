#include "RobotLibrary.h"

RobotLibrary::RobotLibrary(): robot()
{
	bool rep = robot.ConnectToRobot(IP_ADRESSE, PORT);
		if (rep)
		{
			printf("Connection failed...\n");
			getchar();
			return;
		}
		else
		{
			printf("Connection established...\n");
		}
		
	//initialisation odometrie
	old_r = sensors_data.OdometryRight;
	old_l = sensors_data.OdometryLeft;
	x = 0;
	y = 0;
}

void RobotLibrary::move(int right, int left) {
	unsigned char flags = 128 + 32 + ((left>=0) ? 1 : 0) * 64 + ((right>=0) ? 1 : 0)*16 + 1;
	int r, l;
	
	r = abs((int)(255 * right / 100.f));
	l = abs((int)(255 * left / 100.f));
	robot.SendCommand(r, l, flags);
}

void RobotLibrary::processOdometry() {
	long r = sensors_data.OdometryRight - old_r;
	long l = sensors_data.OdometryLeft - old_l;
	long d = r + l; // distance parcourue
	double alpha = atan(r/l - 1);
	x += d*cos(alpha);
	y += d*sin(alpha);
	
	// doivent etre remis a 0
	old_r = sensors_data.OdometryRight;
	old_l = sensors_data.OdometryLeft;
}

double RobotLibrary::getX() {
	return x;
}

double RobotLibrary::getY() {
	return y;
}

void RobotLibrary::updateSensorDate() {
	robot.GetSensorData(&sensors_data);
	
	processOdometry();
}

bool RobotLibrary::obstableDroite() {
	if (sensors_data.IRRight > THRESH_RIGHT)
		return true;
	else
		return false;
}

bool RobotLibrary::obstacleGauche() {
	if (sensors_data.IRLeft > THRESH_LEFT)
		return true;
	else
		return false;
}

void RobotLibrary::forward(){
	robot.SendCommand(140, 140, FLAG_FORWARD_DEFAULT);
	}
	
void RobotLibrary::backward(){
	
	robot.SendCommand(148, 148, FLAG_BACKWARD_DEFAULT);
	
	}
	
void RobotLibrary::left(){
	
	robot.SendCommand(125, 125, FLAG_LEFT_DEFAULT);
	
	}
	
void RobotLibrary::right(){
	
	robot.SendCommand(125, 125, FLAG_RIGHT_DEFAULT);
	
	}
	
void RobotLibrary::stop(){
	
	robot.SendCommand(0, 0, FLAG_STOP_DEFAULT);
	
	}

SensorData* RobotLibrary::getData() {
	return &sensors_data;
}
	