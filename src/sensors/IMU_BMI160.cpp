#include "IMU_BMI160.h"
#include <Wire.h>

// センサーのインスタンスを作成
BMI160GenClass BMI160_1;
BMI160GenClass BMI160_2;

bool initBMI160(bool isSecond) {
  bool result;
  // BMI160の初期化(アドレスをもとに切り替え)
  if (isSecond) {
    result = BMI160_2.begin(BMI160GenClass::I2C_MODE, Wire1, BMI160_ADDR2, PIN_INT2);
  } else {
    result = BMI160_1.begin(BMI160GenClass::I2C_MODE, Wire, BMI160_ADDR1, PIN_INT);
  }

  Serial.println("BMI160 Initialized");
  return true;
}

void readBMI160(bool isSecond, float *ax, float *ay, float *az, float *gx, float *gy, float *gz) {
  // センサーが吐き出すrawデータを格納する変数
  int raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz;

  // 1台目か2台目かでインスタンスを切り替え
  if (isSecond) {
    BMI160_2.readMotionSensor(raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz);
  } else {
    BMI160_1.readMotionSensor(raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz);
  }

  // 加速度をgに変換(フルスケールは8g)
  *ax = raw_ax / 32768.0 * 8;
  *ay = raw_ay / 32768.0 * 8;
  *az = raw_az / 32768.0 * 8;

  // ジャイロをdpsに変換(フルスケールは1000dps)
  *gx = raw_gx / 32768.0 * 1000;
  *gy = raw_gy / 32768.0 * 1000;
  *gz = raw_gz / 32768.0 * 1000;
}
