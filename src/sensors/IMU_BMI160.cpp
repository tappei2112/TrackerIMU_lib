#include "IMU_BMI160.h"
#include <Wire.h>
#include <math.h>

// センサーのインスタンスを作成
BMI160GenClass BMI160_1; // 1台目のインスタンス
BMI160GenClass BMI160_2; // 2台目のインスタンス

bool initBMI160(bool isSecond) {
  bool result;
  // BMI160の初期化(アドレスをもとに切り替え)
  if (isSecond) {
    result = BMI160_2.begin(BMI160GenClass::I2C_MODE, Wire1, BMI160_ADDR2, PIN_IMU_INT_2);
  } else {
    result = BMI160_1.begin(BMI160GenClass::I2C_MODE, Wire, BMI160_ADDR1, PIN_IMU_INT);
  }

  Serial.println("BMI160 Initialized");
  return true;
}

void readBMI160(bool isSecond, float *accel, float *gyro) {
  // センサーが吐き出すrawデータを格納する変数
  int raw_accel[3];
  int raw_gyro[3];

  // 1台目か2台目かでインスタンスを切り替え
  if (isSecond) {
    BMI160_2.readMotionSensor(raw_accel[0], raw_accel[1], raw_accel[2], raw_gyro[0], raw_gyro[1], raw_gyro[2]);
  } else {
    BMI160_1.readMotionSensor(raw_accel[0], raw_accel[1], raw_accel[2], raw_gyro[0], raw_gyro[1], raw_gyro[2]);
  }

  for (int i = 0; i < 3; i++){
  // 加速度をgに変換(フルスケールは8g)
  accel[i] = raw_accel[i] / 32768.0 * 8;
  // ジャイロをdpsに変換(フルスケールは1000dps)
  gyro[i] = raw_gyro[i] / 32768.0 * 1000;
  }
}
