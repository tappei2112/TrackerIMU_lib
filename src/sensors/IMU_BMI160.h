#ifndef IMU_BMI160_H
#define IMU_BMI160_H
// EmotBit BMI160ライブラリを使用
#include <BMI160Gen.h>
#include "define.h"

/* ライブラリ bmi160.cppの書き換え(bmi160.cpp L116/L117)
setFullScaleGyroRange(BMI160_GYRO_RANGE_1000);
setFullScaleAccelRange(BMI160_ACCEL_RANGE_8G);
*/

// 初期化
bool initBMI160(bool isSecond);
// 値の読み取り
void readBMI160(bool isSecond, float *accel, float *gyro);

#endif // IMU_BMI160_H
