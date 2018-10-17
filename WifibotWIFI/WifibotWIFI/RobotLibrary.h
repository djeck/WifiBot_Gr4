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
#define E 25 // Espacement entre les deux rangés de roue

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
	WifibotClient* getClient();
	
	double getX();
	double getY();
	double getOrientation();
private: 
	WifibotClient robot;
	SensorData sensors_data;
	double x, y;
	long old_r, old_l;
	double orientation;
	int compteur;
};