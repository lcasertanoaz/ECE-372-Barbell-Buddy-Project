#ifndef IMU_H
#define IMU_H

void initIMU(void);
void calibrateIMU(void);
void updateTiltEstimate(void);
float getTiltAngleDeg(void);

#endif