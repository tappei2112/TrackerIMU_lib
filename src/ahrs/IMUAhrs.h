#ifndef IMUAhrs_H_
#define IMUAhrs_H_
#include <math.h>
#include "MahonyAHRS.h"

void IMUcalcAhrs(float *accel, float *gyro, float *angle) {
    MahonyAHRSupdateIMU(gyro[0] * PI / 180, gyro[1] * PI / 180, gyro[2] * PI / 180, accel[0], accel[1], accel[2], angle);
}

#endif // IMUAhrs_H_
