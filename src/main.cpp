#include <Arduino.h>
#include "define.h"
#include "sensors/IMU_BMI160.h"

// タスクハンドル
TaskHandle_t Task1;
TaskHandle_t Task2;
// 演算結果をコアごとに格納する文字列
String result1 = "";
String result2 = "";
// どうやら同時に初期化処理をすることに問題がありそうなので、一方の初期化を待つための変数
bool PInited = false;
bool SInited = false;

class Tracker{
public:
  float ini_gyro[3] = {0.0, 0.0, 0.0};
  float accel[3] = {0.0, 0.0, 0.0};
  float gyro[3] = {0.0, 0.0, 0.0};
  float angle[3] = {0.0, 0.0, 0.0}; // pitch, roll, yaw
  float world_accel[3] = {0.0, 0.0, 0.0};
  float world_velocity[3] = {0.0, 0.0, 0.0};
  float world_position[3] = {0.0, 0.0, 0.0};
  unsigned long last_time = 0;
  unsigned long current_time = millis();
  float dt = 0;  // ミリ秒から秒に変換


  // IMUの初期化(将来的にセンサーを変更しても対応できるようにしている)
  bool IMU_Init(int kind, bool isSecond){ //使用するIMUに合わせた初期化処理を呼び出す
    bool result;
    switch(kind){
      case IMU_BMI160:
        result = initBMI160(isSecond);
        break;
      default:
        break;
    }
    return result;
  }

  // 値の読み取り(将来的にセンサーを変更しても対応できるようにしている)
  void IMU_Read(int kind, bool isSecond){ //使用するIMUに合わせた値の読み出し処理を、変数を間接参照して呼び出す
    switch(kind){
      case IMU_BMI160:
        readBMI160(isSecond, accel, gyro);
        break;
      default:
        break;
    }
  }

  // 角度の計算
  void calculateAngles(float dt) {
    float alpha = 0.98; // フィルター係数
    float comp_angle[2] = {0.0, 0.0};

    // 加速度データからピッチとロールを計算
    float pitch_accel = atan2(accel[1], sqrt(accel[0] * accel[0] + accel[2] * accel[2])) * 180 / PI;
    float roll_accel = atan2(-accel[0], accel[2]) * 180 / PI;

    // ジャイロデータから姿勢角度を計算
    for (int i = 0; i < 3; i++){
      if(abs(gyro[i]) > 1.5){
        angle[i] += gyro[i] * dt;
      }
    }

    // コンプリメンタリフィルターでデータを融合
    comp_angle[0] = alpha * (comp_angle[0] + angle[0]) + (1 - alpha) * pitch_accel;
    comp_angle[1] = alpha * (comp_angle[1] + angle[1]) + (1 - alpha) * roll_accel;

    // 足モード(mode = 0)でほぼ下向きの時、角度のずれを修正
    if(abs(comp_angle[0]) < 10 && abs(comp_angle[1]) < 10){
      for(int i = 0; i < 3; i++){
        angle[i] = 0;
      }
    }

    //あまり足が上がっていないときはコンプリメンタルフィルタ適用
    if(abs(comp_angle[0]) < 30 && abs(comp_angle[1]) < 30){
      for(int i = 0; i < 2; i++){
        angle[i] =  comp_angle[i];
      }
    }
  }

  // 位置の計算
  /*void calculatePositions(){
    float radAngle[3];
    //float absaccel = sqrt(accel[0] * accel[0] + accel[1] * accel[1] + accel[2] * accel[2]);
    float absaccel = 9.81;
    float gravity[3];
    float dynamic_accel[3];
    float rotation[3][3];

    // 三角関数を計算するために、角度をラジアンに
    for (int i = 0; i < 3; i++){
      radAngle[i] = angle[i] * M_PI / 180.0;
    }

    // 加速度から重力成分を除去
    gravity[0] = absaccel * sin(radAngle[0]);
    gravity[1] = absaccel * -sin(radAngle[1]) * cos(radAngle[0]);
    gravity[2] = absaccel * cos(radAngle[1]) * cos(radAngle[0]);

    for (int i = 0; i < 3; i++){
      dynamic_accel[i] = accel[i] * 9.81 - gravity[i];
    }

    // 回転行列の計算
    float R_x[3][3] = {
        {1, 0, 0},
        {0, cos(radAngle[1]), -sin(radAngle[1])},
        {0, sin(radAngle[1]), cos(radAngle[1])}
    };

    float R_y[3][3] = {
        {cos(radAngle[0]), 0, sin(radAngle[0])},
        {0, 1, 0},
        {-sin(radAngle[0]), 0, cos(radAngle[0])}
    };

    float R_z[3][3] = {
        {cos(radAngle[2]), -sin(radAngle[2]), 0},
        {sin(radAngle[2]), cos(radAngle[2]), 0},
        {0, 0, 1}
    };

    // 回転行列 R_y * R_x を計算
    float R_yx[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R_yx[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                R_yx[i][j] += R_y[i][k] * R_x[k][j];
            }
        }
    }

    // 最終的な回転行列 R = R_z * R_yx を計算
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rotation[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                rotation[i][j] += R_z[i][k] * R_yx[k][j];
            }
        }
    }

    // ワールド座標系の加速度(m/s^2)を計算
    for (int i = 0; i < 3; i++) {
        world_accel[i] = 0.0;
        for (int j = 0; j < 3; j++) {
          world_accel[i] += rotation[i][j] * dynamic_accel[j];
        }
    }

    // 2重積分
    integrate_matrix(world_accel, world_velocity, dt);
    integrate_matrix(world_velocity, world_position, dt);
  }

  // 3要素配列の積分関数
  void integrate_matrix(float *data, float *target, float dt){
    for (int i = 0; i < 3; i++) {
      target[i] += data[i] * dt;
    }
  }*/

  bool IMU_Print(int i, bool isSecond){
    int j;

    #ifdef SECOND_IMU
        int kind = (isSecond)? SECOND_IMU : IMU;
    #else
        int kind = IMU;
    #endif
    IMU_Read(kind, isSecond);
    current_time = millis();
    dt = (current_time - last_time) / 1000.0;  // ミリ秒から秒に変換
    last_time = current_time;

      calculateAngles(dt);
      //calculatePositions();
      if (i % STEP == 0){
        String result = "";
        #ifdef DEBUG_SENSOR
          if(isSecond){
            result = "Secondary ";
          } else {
            result = "Primary ";
          }
          result += "IMU - Accel: " + String(accel[0]) + ", " + String(accel[1]) + ", " + String(accel[2]);
          result += " | Gyro: " + String(gyro[0]) + ", " + String(gyro[1]) + ", " + String(gyro[2]);
          result += " | Angle: " + String(angle[0]) + ", " + String(angle[1]) + ", " + String(angle[2]);
        #else
          result = String(angle[0]) + ", " + String(angle[1]) + ", " + String(angle[2]);
        #endif
        if(isSecond){
          result2 = result;
        } else {
          result1 = result;
        }
        return true;
      }
      return false;
    }
  //}
};

// Trackerのインスタンスを作成
Tracker tracker1; // 1台目のインスタンス
Tracker tracker2; // 2台目のインスタンス

// 1つ目のIMU制御
void Prime_IMU_func(void *pvParameters) {
  static int i = 0; //実行回数
  while (true) {
    if (tracker1.IMU_Print(i, false)) {
      Serial.print(result1);
      Serial.print(", ");
      Serial.print(result2);
      Serial.println();
    }
    i++;
  }
}

#ifdef SECOND_IMU
// 2つ目のIMU制御
void Second_IMU_func(void *pvParameters) {
  static int i = 0; //実行回数
  while (true) {
    tracker2.IMU_Print(i, true);
    i++;
  }
}
#endif

void setup() {
  // シリアル通信の初期化
  Serial.begin(BAUDRATE);
  // i2cの初期化
  Wire.begin(); // default GPIO21 = SDA, GPIO18 = SCL
  delay(100);
  Wire1.begin(PIN_IMU_SDA_2, PIN_IMU_SCL_2);  // GPIO19 = SDA, GPIO18 = SCL
  // IMUの初期化
  Serial.println("Initializing Primary IMU device...");
  while (!tracker1.IMU_Init(IMU, false)) {
    Serial.println("Primary IMU initialization failed.");
    delay(1000); // Retry delay
  }
  Serial.println("Primary IMU Initialized");
  delay(100);
  #ifdef SECOND_IMU
  Serial.println("Initializing Secondary IMU device...");
  while (!tracker2.IMU_Init(IMU_BMI160, true)) {
    Serial.println("Secondary IMU initialization failed.");
    delay(1000); // Retry delay
  }
  Serial.println("Secondary IMU Initialized");
  #endif

    // コア0でタスクを作成
  xTaskCreatePinnedToCore(Prime_IMU_func, "Prime_IMU_func", 4096, NULL, 1, &Task1, 1);
#ifdef SECOND_IMU
  xTaskCreatePinnedToCore(Second_IMU_func, "Second_IMU_func", 4096, NULL, 1, &Task2, 0);
#endif
}

void loop() {
  // 特に何もしない
}
