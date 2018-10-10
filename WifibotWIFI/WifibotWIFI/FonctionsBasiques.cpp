#include <stdio.h>
#include <stdlib.h>

#include "WifibotClient.h"
#include "FonctionsBasiques.h"

void avancer(WifibotClient *robot, int right, int left) {
	unsigned char flags = 128 + 32 + ((left>=0) ? 1 : 0) * 64 + ((right>=0) ? 1 : 0)*16 + 1;
	int r, l;
	
	r = abs((int)(255 * right / 100.f));
	l = abs((int)(255 * left / 100.f));
	robot->SendCommand(r, l, flags);
}

bool obstableDroite(SensorData *sensors_data) {
	if (sensors_data->IRRight > THRESH_RIGHT)
		return true;
	else
		return false;
}



bool obstacleGauche(SensorData *sensors_data) {
	if (sensors_data->IRLeft > THRESH_LEFT)
		return true;
	else
		return false;
}