#ifndef IMU_BMI160_H
#define IMU_BMI160_H
// EmotBit BMI160ライブラリを使用
#include <BMI160Gen.h>
#include "define.h"

#define BMI160_ADDR1 0x69 // 1台目用のアドレス
#define BMI160_ADDR2 0x68 // 2台目用のアドレス

// 初期化
bool initBMI160(bool isSecond);
// 値の読み取り
void readBMI160(bool isSecond, float *accel, float *gyro);

#endif // IMU_BMI160_H
