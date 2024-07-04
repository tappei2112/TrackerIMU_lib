#ifndef DEFINE_H_
#define DEFINE_H_
#include "consts.h"

// 使用するIMUとボードの設定
#define IMU IMU_BMI160
//#define SECOND_IMU IMU_BMI160
#define BOARD BOARD_WROOM32
#define IMU_ROTATION DEG_270
#define SECOND_IMU_ROTATION DEG_270

// ボードごとの設定
#if BOARD == BOARD_WROOM32
  // シリアル通信の速度
  #define BAUDRATE 115200
  // 1台目のIMUの設定
  #define PIN_IMU_SDA 21
  #define PIN_IMU_SCL 22
  #define PIN_IMU_INT 23
  // 2台目のIMUの設定
  #define PIN_IMU_SDA_2 19
  #define PIN_IMU_SCL_2 18
  #define PIN_IMU_INT_2 25
#endif

#endif // DEFINE_H_
