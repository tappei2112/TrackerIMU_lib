#ifndef BMI160_H
#define BMI160_H
// EmotBit BMI160ライブラリを使用
#include <BMI160Gen.h>

#define BMI160_ADDR1 0x69 // 1台目用のアドレス
#define BMI160_ADDR2 0x68 // 2台目用のアドレス

#define PIN_INT 23 // 1台目用のIntピン
#define PIN_INT2 25 // 2台目用のIntピン

// 初期化
bool initBMI160(bool isSecond);
// 値の読み取り
void readBMI160(bool isSecond, float *ax, float *ay, float *az, float *gx, float *gy, float *gz);

#endif // BMI160_H
