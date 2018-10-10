#pragma once
#ifndef    FONCTIONS_H
#define    FONCTIONS_H

#define THRESH_RIGHT 30
#define THRESH_LEFT 30

void avancer(WifibotClient *robot, int right, int left);
bool obstableDroite(SensorData *sensors_data);
bool obstacleGauche(SensorData *sensors_data);

#endif