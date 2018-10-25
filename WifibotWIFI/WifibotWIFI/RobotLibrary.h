#include "WifibotClient.h"

//#pragma once 

#define WHEELS_FORWARD 50
#define WHEELS_BACKWARD -50
#define WHEELS_DEFAULT 0

#define UINT8_t unsigned char 
#define IP_ADRESSE "192.168.1.77"
#define PORT	15020

#define THRESH_RIGHT 30 // seuils infrarouge pour la detection d'obstacle
#define THRESH_LEFT 30
#define E 25 // Espacement entre les deux rang�s de roue

enum side_IRSens { RIGHT, LEFT };

/*
 * @brief Bibliotheques fournissant les fonctionnalites de base
 * du robot (deplacement, capteurs, ...)
 */
class RobotLibrary {
public: 
	/*
	 * @brief Connection au robot et initialisation
	 */
	RobotLibrary();
	/*
	 * @brief Reception des donnees des capteurs
	 */
	void updateSensorData();
	/*
	 * @brief Deplacement du robot
	 * @arg right vitesse de rotation de la roue droite
	 * @arg left vitesse de rotation de la roue gauche
	 */
	void move(int right, int left);
	/*
	 * @brief Detection d'obstacle avec le capteur IR droit
	 * @return true si un obstacle est detecter, false sinon
	 */
	bool obstableDroite();
	/*
	 * @brief Detection d'obstacle avec le capteur IR gauche
	 * @return true si un obstacle est detecter, false sinon
	 */
	bool obstacleGauche();
	void forward();
	void backward();
	void left();
	void right();
	void stop();
	SensorData* getData();
	void processOdometry();
	void processOdometry2();
	WifibotClient* getClient();
	
	double getX();
	double getY();
	double getOrientation();
	int getDistance(enum side_IRSens side, int *LUT);
private: 
	WifibotClient robot;
	SensorData sensors_data;
	double x = 0;
	double y = 0;
	long old_r, old_l;
	double orientation = 0;
	int compteur;

	double old_odoG = 0;
	double old_odoD = 0;

	double meas_odoG = 0;
	double meas_odoD = 0;

	double radG = 0;
	double radD = 0;
};
