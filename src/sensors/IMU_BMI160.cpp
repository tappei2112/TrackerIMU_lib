#include "IMU_BMI160.h"
#include <Wire.h>
#include <math.h>

// センサーのインスタンスをポインタで管理
BMI160GenClass* BMI160_1 = nullptr; // 1台目のポインタ
BMI160GenClass* BMI160_2 = nullptr; // 2台目のポインタ

bool initBMI160(bool isSecond) {
  Serial.println("BMI160 Initializing");

  // ポインタを使用してBMI160の初期化
  BMI160GenClass* currentBMI160 = isSecond ? BMI160_2 : BMI160_1;
  #ifdef SECOND_IMU
    TwoWire* currentWire = isSecond ? &Wire : &Wire1;
    int currentAddress = isSecond ? BMI160_ADDR2 : BMI160_ADDR1;
    int currentIntPin = isSecond ? PIN_IMU_INT_2 : PIN_IMU_INT;
  #else
    TwoWire* currentWire = &Wire1;
    int currentAddress = BMI160_ADDR1;
    int currentIntPin = PIN_IMU_INT;
  #endif

  // インスタンスが初期化されていなければ新たに作成
  if (currentBMI160 == nullptr) {
    currentBMI160 = new BMI160GenClass();
  }

  // 初期化プロセス
  if (!currentBMI160->begin(BMI160GenClass::I2C_MODE, *currentWire, currentAddress, currentIntPin)) {
    Serial.println(isSecond ? "BMI160_2 Initialization failed!" : "BMI160_1 Initialization failed!");
    return false;
  }

  // ポインタを更新
  if (isSecond) {
    BMI160_2 = currentBMI160;
  } else {
    BMI160_1 = currentBMI160;
  }

  Serial.println("BMI160 Initialized");
  return true;
}

void readBMI160(bool isSecond, float *accel, float *gyro) {
  // 現在のセンサーを選択
  BMI160GenClass* currentBMI160 = isSecond ? BMI160_2 : BMI160_1;

  // センサーが吐き出すrawデータを格納する変数
  int raw_accel[3];
  int raw_gyro[3];

  // モーションセンサーのデータ読み取り
  currentBMI160->readMotionSensor(raw_accel[0], raw_accel[1], raw_accel[2], raw_gyro[0], raw_gyro[1], raw_gyro[2]);

  // 加速度とジャイロスコープデータの変換
  for (int i = 0; i < 3; i++) {
    accel[i] = raw_accel[i] / 32768.0 * 8;  // 加速度をgに変換
    gyro[i] = raw_gyro[i] / 32768.0 * 1000; // ジャイロをdpsに変換
  }
}
