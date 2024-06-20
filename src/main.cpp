#include <Arduino.h>
#include "define.h"
#include "consts.h"
#include "sensors/IMU_BMI160.h"

bool IMU_Init(int, bool);
void IMU_Read(int, bool, float*, float*, float*, float*, float*, float*);

void setup() {
  // シリアル通信の初期化
  Serial.begin(115200);

  // i2cの初期化
  Wire.begin(); // default GPIO21 = SDA, GPIO18 = SCL
  Wire1.begin(19, 18);  // GPIO19 = SDA, GPIO18 = SCL


  // IMUの初期化
  Serial.println("Initializing IMU device...");
  if (IMU_Init(IMU_BMI160, false)) {
    Serial.println("Primary IMU Initialized");
  } else {
    Serial.println("Failed to initialize Primary IMU");
  }

  if (IMU_Init(IMU_BMI160, true)) {
    Serial.println("Secondary IMU Initialized");
  } else {
    Serial.println("Failed to initialize Secondary IMU");
  }
}

void loop() {
    float ax, ay, az;
    float gx, gy, gz;

    IMU_Read(IMU, false, &ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("Primary IMU - Accel: ");
    Serial.print(ax);
    Serial.print(", ");
    Serial.print(ay);
    Serial.print(", ");
    Serial.print(az);
    Serial.print(" | Gyro: ");
    Serial.print(gx);
    Serial.print(", ");
    Serial.print(gy);
    Serial.print(", ");
    Serial.print(gz);
    Serial.println();

    IMU_Read(SECOND_IMU, true, &ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("Secondary IMU - Accel: ");
    Serial.print(ax);
    Serial.print(", ");
    Serial.print(ay);
    Serial.print(", ");
    Serial.print(az);
    Serial.print(" | Gyro: ");
    Serial.print(gx);
    Serial.print(", ");
    Serial.print(gy);
    Serial.print(", ");
    Serial.print(gz);
    Serial.println();

    delay(500);
}

// IMUの初期化(将来的にセンサーを変更しても対応できるようにしている)
bool IMU_Init(int kind, bool isSecond){ //使用するIMUに合わせた初期化処理を呼び出す
  switch(kind){
    case IMU_BMI160:
      initBMI160(isSecond);
      break;
    default:
      break;
  }
  return true;
}

// 値の読み取り(将来的にセンサーを変更しても対応できるようにしている)
void IMU_Read(int kind, bool isSecond, float *ax, float *ay, float *az, float *gx, float *gy, float *gz){ //使用するIMUに合わせた値の読み出し処理を、変数を間接参照して呼び出す
  switch(kind){
    case IMU_BMI160:
      readBMI160(isSecond, ax, ay, az, gx, gy, gz);
      break;
    default:
      break;
  }
}
