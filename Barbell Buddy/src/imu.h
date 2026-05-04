#ifndef IMU_H
#define IMU_H

void initIMU(void);
void calibrateIMU(void);
void updateTiltEstimate(void);
float getTiltAngleDeg(void);
float getXAccelG(void);
float getYAccelG(void);
float getZAccelG(void);

#endif