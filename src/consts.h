// SlimeVRのファームウェアを参考にしています
#ifndef CONSTS_H_
#define CONSTS_H_

// 使用するIMUの種類
#define IMU_UNKNOWN 0
//#define IMU_MPU9250 1
//#define IMU_MPU6500 2
//#define IMU_BNO080 3
#define IMU_BNO085 4
//#define IMU_BNO055 5
#define IMU_MPU6050 6
//#define IMU_BNO086 7
#define IMU_BMI160 8
//#define IMU_ICM20948 9
//#define IMU_ICM42688 10
#define IMU_DEV_RESERVED 250 // Reserved, should not be used in any release firmware
#define IMU_NONE 255 // IMUがつながっていない

// 使用するマイコンの種類
#define BOARD_UNKNOWN 0
//#define BOARD_SLIMEVR_LEGACY 1
//#define BOARD_SLIMEVR_DEV 2
//#define BOARD_NODEMCU 3
#define BOARD_CUSTOM 4
#define BOARD_WROOM32 5
//#define BOARD_WEMOSD1MINI 6
//#define BOARD_TTGO_TBASE 7
//#define BOARD_ESP01 8
//#define BOARD_SLIMEVR 9
//#define BOARD_LOLIN_C3_MINI 10
//#define BOARD_BEETLE32C3 11
#define BOARD_ES32C3DEVKITM1 12
//#define BOARD_OWOTRACK 13 // Only used by owoTrack mobile app
//#define BOARD_WRANGLER 14 // Only used by wrangler app
//#define BOARD_MOCOPI 15 // Used by mocopi/moslime
//#define BOARD_WEMOSWROOM02 16
//#define BOARD_XIAO_ESP32C3 17
//#define BOARD_DEV_RESERVED 250 // Reserved, should not be used in any release firmware
#define BOARD_M5CORE2 18

// 角度の設定
#define HORIZON 0 // 水平
#define VERT_F 1 //垂直(体の前面に取り付け)
#define VERT_B 2 //垂直(体の後面に取り付け)
#define VERT_L 3 //垂直(体の左面に取り付け)
#define VERT_R 4 //垂直(体の右面に取り付け)

#define CONST_EARTH_GRAVITY 9.80665

/*#define ACCEL_CALIBRATION_METHOD_SKIP 0
#define ACCEL_CALIBRATION_METHOD_ROTATION 1
#define ACCEL_CALIBRATION_METHOD_6POINT 2

#define BMI160_MAG_TYPE_HMC 1
#define BMI160_MAG_TYPE_QMC 2

#define MCU_UKNOWN 0
#define MCU_ESP8266 1
#define MCU_ESP32 2
#define MCU_OWOTRACK_ANDROID 3 // Only used by owoTrack mobile app
#define MCU_WRANGLER 4 // Only used by wrangler app
#define MCU_OWOTRACK_IOS 5 // Only used by owoTrack mobile app
#define MCU_ESP32_C3 6
#define MCU_MOCOPI 7 // Used by mocopi/moslime
#define MCU_DEV_RESERVED 250 // Reserved, should not be used in any release firmware

#ifdef ESP8266
  #define HARDWARE_MCU MCU_ESP8266
#elif defined(ESP32)
  #define HARDWARE_MCU MCU_ESP32
#else
  #define HARDWARE_MCU MCU_UKNOWN
#endif

#define CURRENT_CONFIGURATION_VERSION 1*/

#endif // SLIMEVR_CONSTS_H_
