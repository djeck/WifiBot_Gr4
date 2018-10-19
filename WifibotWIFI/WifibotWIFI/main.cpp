#include <stdio.h>
#include <stdlib.h>

#include "WifibotClient.h"
#include "RobotLibrary.h"

int SharpLUT[]={150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,103,102,101,100,99,
          96,95,90,88,87,86,85,84,83,82,81,80,79,77,75,74,73,72,71,70,69,68,67,66,65,65,64,64,63,62,61,61,60,60,59,59,58,58,57,57,56,56,56,55,55,55,54,54,54,53,
          53,53,52,52,51,51,50,50,49,49,49,48,48,47,47,46,46,46,45,45,44,44,43,43,42,42,41,41,41,40,40,40,39,39,39,39,39,38,38,
                  38,38,37,37,37,37,36,36,36,36,35,35,35,35,34,34,34,34,34,34,33,33,33,33,32,32,32,32,32,31,31,31,31,30,30,30,30,30,30,30,
          30,29,29,29,29,29,29,29,28,28,28,28,28,28,27,27,27,27,27,27,27,26,26,26,26,25,25,25,25,25,25,24,24,24,24,24,23,23,23,
          23,23,23,22,22,22,22,22,21,21,21,21,21,21,20,20,20,20,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,16,16,15,15};

void main(void)
{
	/*.........................*/
	/* Connection to the robot */
	/*.........................*/
	RobotLibrary library;
	int i = 0;

	/*..............*/
	/* Main program */
	/*..............*/

	while (1)
	{
		library.updateSensorData();
		//library.move(0, 0);
		//printf("Batterie : %d\n", library.getData()->BatVoltage); 
		system("cls");
		printf("===== data %d: \n", i);
		printf("Capteur IR distance gauche : %dcm\n", library.getDistance(LEFT, SharpLUT));
		printf("Capteur IR distance droit : %dcm\n", library.getDistance(RIGHT, SharpLUT));
		/*printf("Capteur IR distance gauche : %dcm\n", SharpLUT[library.getData()->IRLeft]);
		printf("Capteur IR distance droit : %dcm\n", SharpLUT[library.getData()->IRRight]);
		/*printf("Vitesse droite : %d\n", library.getData()->SpeedFrontRight);
		printf("Odometrie roue droite : %ld\n", library.getData()->OdometryRight);
		printf("Odometrie roue gauche : %ld\n", library.getData()->OdometryLeft);
		printf("X : %f\n", library.getX());
		printf("Y : %f\n", library.getY());

		if (library.obstacleGauche() && library.obstableDroite())
			library.move(0, 0);
		else if (library.obstableDroite())
			library.move(50, 0);
		else if (library.obstacleGauche())
			library.move(0, 50);

		*/
		//library.obstacleGauche()

		/***
		if ((library.getDistance(LEFT, SharpLUT) <= 80 && library.getDistance(LEFT, SharpLUT) > 60) || (library.getDistance(RIGHT, SharpLUT) <= 80 && library.getDistance(RIGHT, SharpLUT) < 60))
		{
			library.move(50, 50);
		}
		else if ((library.getDistance(LEFT, SharpLUT) <= 60 && library.getDistance(LEFT, SharpLUT) > 45) || (library.getDistance(RIGHT, SharpLUT) <= 60 && library.getDistance(RIGHT, SharpLUT) < 45))
		{
			library.move(20, 20);
		}
		***/
		if ((library.getDistance(LEFT, SharpLUT) <= 60) && (library.getDistance(RIGHT, SharpLUT) <= 60))
		{
			library.move(0, 0);
			printf("STOP");
		}
		else if ((library.getDistance(LEFT, SharpLUT) <= 60) && (library.getDistance(RIGHT, SharpLUT) >= 60))
		{
			library.move(-30, 30);
			printf("LEFT");
		}
		else if ((library.getDistance(LEFT, SharpLUT) >= 60) && (library.getDistance(RIGHT, SharpLUT) <= 60))
		{
			library.move(30, -30);
			printf("RIGHT");
		}
		else if ((library.getDistance(LEFT, SharpLUT) > 60) || (library.getDistance(RIGHT, SharpLUT) > 60))
		{
			library.move(30, 30);
			printf("FORWARD");
		}

		i++;
	}
}


