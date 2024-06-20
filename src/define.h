// Set parameters of IMU and board used
#define IMU IMU_BMI160
#define SECOND_IMU IMU_BMI160
#define BOARD BOARD_WROOM32
#define IMU_ROTATION DEG_270
#define SECOND_IMU_ROTATION DEG_270

// Board-specific configurations
#if BOARD == BOARD_WROOM32
  #define PIN_IMU_SDA 21
  #define PIN_IMU_SCL 22
  #define PIN_IMU_INT 23 // 1台目のINT
  #define PIN_IMU_INT_2 25 // 2台目のINT
  #define PIN_BATTERY_LEVEL 36
#endif
