#include <Arduino.h>
#include "define.h"
#include "sensors/IMU_BMI160.h"
//#include <esp_system.h>

void applyRotationMatrix(bool, float*, float*);

// タスクハンドル
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
// 演算結果をコアごとに格納する文字列
String result1 = "0.00, 0.00, 0.00";
String result2 = "0.00, 0.00, 0.00";

class Tracker{
public:
  float accel[3] = {0.0, 0.0, 0.0};
  float gyro[3] = {0.0, 0.0, 0.0};
  float angle[3] = {0.0, 0.0, 0.0}; // pitch, roll, yaw
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
    float default_accel[3] = {0.0, 0.0, 0.0};
    float default_gyro[3] = {0.0, 0.0, 0.0};
    switch(kind){
      case IMU_BMI160:
        readBMI160(isSecond, default_accel, default_gyro);
        break;
      default:
        break;
    }
    applyRotationMatrix(isSecond, default_accel, accel);
    applyRotationMatrix(isSecond, default_gyro, gyro);
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
    if(abs(comp_angle[0]) + abs(comp_angle[1]) < MAX_NOYAW){
      for(int i = 0; i < 3; i++){
        angle[i] = 0;
      }
    }

    //あまり足が上がっていないときはコンプリメンタルフィルタ適用
    if(abs(comp_angle[0]) < MAX_COMP && abs(comp_angle[1]) < MAX_COMP){
      for(int i = 0; i < 2; i++){
        angle[i] =  comp_angle[i];
      }
    }
  }

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
};

// Trackerのインスタンスを作成
Tracker tracker1; // 1台目のインスタンス
Tracker tracker2; // 2台目のインスタンス

// 1つ目のIMU制御タスク
void Prime_IMU_Task(void *pvParameters) {
  static int i = 0; //実行回数
  while (true) {
    tracker1.IMU_Print(i, false);
    i++;
    delay(5); // センサー読み取りのための短い遅延
  }
}

// 2つ目のIMU制御タスク
void Second_IMU_Task(void *pvParameters) {
  static int i = 0; //実行回数
  while (true) {
    #ifdef SECOND_IMU
    tracker2.IMU_Print(i, true);
    #endif
    i++;
    delay(5); // センサー読み取りのための短い遅延
  }
}

// シリアル通信監視タスク
void SerialMonitorTask(void *pvParameters) {
  while (true) {
    if (Serial.available() > 0) {
      String receivedData = Serial.readStringUntil('\n');  // 改行までの文字列を読み取る
      if (receivedData == "SEND") {
        Serial.print(result1);
        Serial.print(", ");
        Serial.print(result2);
        Serial.println();
      } else if (receivedData == "RESET") {
        ESP.restart();
      }
    }
    delay(10);  // シリアル通信監視のための短い遅延
  }
}

void setup() {
  // シリアル通信の初期化
  Serial.begin(BAUDRATE);
  // i2cの初期化
  Wire.begin(); // default GPIO21 = SDA, GPIO22 = SCL
  delay(100);
  Wire1.begin(PIN_IMU_SDA, PIN_IMU_SCL);  // GPIO19 = SDA, GPIO18 = SCL
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

  // タスクを作成して特定のコアに割り当てる
  xTaskCreatePinnedToCore(Prime_IMU_Task, "Prime_IMU_Task", 4096, NULL, 1, &Task1, 1);
  #ifdef SECOND_IMU
  xTaskCreatePinnedToCore(Second_IMU_Task, "Second_IMU_Task", 4096, NULL, 1, &Task2, 0);
  #endif
  xTaskCreatePinnedToCore(SerialMonitorTask, "SerialMonitorTask", 2048, NULL, 1, &Task3, 1);
}

void loop() {
  // 特に何もしない
}

void applyRotationMatrix(bool isSecond, float *array1, float *array2){
  for (int i = 0; i < 3; i++) {
      array2[i] = 0;
    for (int j = 0; j < 3; j++) {
      if(isSecond){
        array2[i] += rotation_matrix_S[i][j] * array1[j];
      } else {
        array2[i] += rotation_matrix_P[i][j] * array1[j];
      }
    }
  }
}
