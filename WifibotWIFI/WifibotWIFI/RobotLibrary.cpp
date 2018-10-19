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
		old_r = 0;// sensors_data.OdometryRight;
		old_l = 0;// sensors_data.OdometryLeft;
	x = 0.0;
	y = 0.0;
	orientation=0.0;
	compteur = 0;
}

void RobotLibrary::move(int right, int left) {
	unsigned char flags = 128 + 32 + ((left>=0) ? 1 : 0) * 64 + ((right>=0) ? 1 : 0)*16 + 1;
	int r, l;
	
	r = abs((int)(255 * right / 100.f));
	l = abs((int)(255 * left / 100.f));
	robot.SendCommand(r, l, flags);
}

WifibotClient* RobotLibrary::getClient()
{
	return &robot;
}

void RobotLibrary::processOdometry() {
	compteur++;
	long r = sensors_data.OdometryRight - old_r;
	long l = sensors_data.OdometryLeft - old_l;
	if (r < 0)
		r = sensors_data.OdometryRight;
	if (l < 0)
		l = sensors_data.OdometryLeft;
	if (compteur < 2) {
		l = 0;
		r = 0;
	}

	double d = ((double) r + (double) l)/2.0; // distance parcourue
	if (r == l) {
		l += 1;
	}
	double R = (E / 2 * (r + l) / (r - l));
	double alpha = d/R;
	/*if (l == 0)
		alpha = -3.141592654 / 2;
	else
		alpha = atan(r/l - 1);*/
	orientation += alpha;
	printf("l : %ld\n", l);
	printf("r : %ld\n", r);
	printf("alpha : %f\n", alpha);
	printf("Orientation : %f\n", orientation);
	x += d*cos(orientation);
	y += d*sin(orientation);
	
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

double RobotLibrary::getOrientation() {
	return orientation;
}

void RobotLibrary::updateSensorData() {
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

void RobotLibrary::forward() {
	//robot.SendCommand(90, 90, FLAG_FORWARD_DEFAULT);
	move(WHEELS_FORWARD, WHEELS_FORWARD);
}

void RobotLibrary::backward() {

	//robot.SendCommand(148, 148, FLAG_BACKWARD_DEFAULT);
	move(WHEELS_BACKWARD, WHEELS_BACKWARD);

}

void RobotLibrary::left() {

	//robot.SendCommand(125, 125, FLAG_LEFT_DEFAULT);
	move(WHEELS_BACKWARD, WHEELS_FORWARD);

}

void RobotLibrary::right() {
	//robot.SendCommand(125, 125, FLAG_RIGHT_DEFAULT);
	move(WHEELS_FORWARD, WHEELS_BACKWARD);

}

void RobotLibrary::stop() {

	move(WHEELS_DEFAULT, WHEELS_DEFAULT);

}

int RobotLibrary::getDistance(side_IRSens side, int *LUT)
{
	if (side == LEFT)
	{
		return LUT[sensors_data.IRLeft];
	}
	else
	{
		return LUT[sensors_data.IRRight];
	}
}

SensorData* RobotLibrary::getData() {
	return &sensors_data;
}
	
