#include <Arduino.h>
#include "define.h"
#include "sensors/IMU_BMI160.h"
#include "ahrs/IMUAhrs.h"
#include "filterIMU.h"

bool IMU_Init(int, bool);
void IMU_Read(int, bool, float*, float*);
void calculateAngles(float*, float*, float, float*);

float angle[3] = {0.0, 0.0, 0.0}; // pitch, roll, yaw
unsigned long last_time = 0;

void setup() {
  // シリアル通信の初期化
  Serial.begin(BAUDRATE);

  // i2cの初期化
  Wire.begin(); // default GPIO21 = SDA, GPIO18 = SCL
  Wire1.begin(PIN_IMU_SDA_2, PIN_IMU_SCL_2);  // GPIO19 = SDA, GPIO18 = SCL


  // IMUの初期化
  Serial.println("Initializing IMU device...");
  if (IMU_Init(IMU_BMI160, false)) {
    Serial.println("Primary IMU Initialized");
  } else {
    Serial.println("Failed to initialize Primary IMU");
  }

#ifdef SECOND_IMU
  if (IMU_Init(IMU_BMI160, true)) {
    Serial.println("Secondary IMU Initialized");
  } else {
    Serial.println("Failed to initialize Secondary IMU");
  }
#endif
}

void loop() {
  float accel[3];
  float gyro[3];
  float previous[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}; // BandPassフィルタの前回値を管理する。前からAccelLowPass, AccelHighPass, GyroLowPass, GyroHighPass
  float alpha = 0.98; // フィルター係数

  unsigned long current_time = millis();
  float dt = (current_time - last_time) / 1000.0;  // ミリ秒から秒に変換
  last_time = current_time;
  static int i = 0;

  IMU_Read(IMU, false, accel, gyro);
  //calculateAngles(accel, gyro, dt, angle);
  for(int i = 0; i < 3; i++){
    bandPassFilter(accel[i], &previous[0][i], &previous[1][i], 0.1, 0.1);
    bandPassFilter(gyro[i], &previous[2][i], &previous[3][i], 0.1, 0.1);
  }

  //IMUcalcAhrs(accel, gyro, angle);
  madgwickUpdate(gyro, accel, angle);
  if (i % 250 == 0){
    Serial.print("Primary IMU - Accel: ");
    Serial.print(accel[0]);
    Serial.print(", ");
    Serial.print(accel[1]);
    Serial.print(", ");
    Serial.print(accel[2]);
    Serial.print(" | Gyro: ");
    Serial.print(gyro[0]);
    Serial.print(", ");
    Serial.print(gyro[1]);
    Serial.print(", ");
    Serial.print(gyro[2]);
    Serial.print(" | Angle: ");
    Serial.print(angle[0]);
    Serial.print(", ");
    Serial.print(angle[1]);
    Serial.print(", ");
    Serial.print(angle[2]);
    Serial.println();
  }

#ifdef SECOND_IMU
  IMU_Read(SECOND_IMU, true, accel, gyro);
  //calculateAngles(accel, gyro, dt, angle);
  IMUcalcAhrs(accel, gyro, angle);
  if (i % 250 == 0){
    Serial.print("Secondary IMU - Accel: ");
    Serial.print(accel[0]);
    Serial.print(", ");
    Serial.print(accel[1]);
    Serial.print(", ");
    Serial.print(accel[2]);
    Serial.print(" | Gyro: ");
    Serial.print(gyro[0]);
    Serial.print(", ");
    Serial.print(gyro[1]);
    Serial.print(", ");
    Serial.print(gyro[2]);
    Serial.print(" | Angle: ");
    Serial.print(angle[0]);
    Serial.print(", ");
    Serial.print(angle[1]);
    Serial.print(", ");
    Serial.print(angle[2]);
    Serial.println();
  }
#endif
  i++;
  //delay(500);
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
void IMU_Read(int kind, bool isSecond, float *accel, float *gyro){ //使用するIMUに合わせた値の読み出し処理を、変数を間接参照して呼び出す
  switch(kind){
    case IMU_BMI160:
      readBMI160(isSecond, accel, gyro);
      break;
    default:
      break;
  }
}

void calculateAngles(float *accel, float *gyro, float dt, float *angle) {
  static float alpha = 0.98; // フィルター係数

  // 加速度データからピッチとロールを計算
  float pitch_accel = atan2(accel[1], sqrt(accel[0] * accel[0] + accel[2] * accel[2])) * 180 / PI;
  float roll_accel = atan2(-accel[0], accel[2]) * 180 / PI;

  // ジャイロデータからピッチとロールを計算
  for (int i = 0; i < 3; i++){
    angle[i] += gyro[i] * dt;
  }

  // コンプリメンタリフィルターでデータを融合
  angle[0] = alpha * (angle[0] + gyro[0] * dt) + (1 - alpha) * pitch_accel;
  angle[1] = alpha * (angle[1] + gyro[1] * dt) + (1 - alpha) * roll_accel;
}
