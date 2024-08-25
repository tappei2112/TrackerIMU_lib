#ifndef DEFINE_H_
#define DEFINE_H_
#include "consts.h"

// 使用するIMUとボードの設定
#define IMU IMU_BMI160
#define SECOND_IMU IMU_BMI160 // コメントアウトした場合、2台目を使用しない
#define BOARD BOARD_WROOM32

// 出力頻度設定(無限ループの実行回数間隔)
#define STEP 40

// コンプリメンタルフィルタ適用角度の最大値
#define MAX_COMP 60

// Yaw角を無視する、PitchとRoll(コンプリメンタルフィルタを使用して算出した値)の和の最大値
#define MAX_NOYAW 20

//#define DEBUG_SENSOR
// ボードごとの設定
#if BOARD == BOARD_WROOM32
  // シリアル通信の速度
  #define BAUDRATE 115200
  // 1台目のIMUの設定
  #define PIN_IMU_SDA 19
  #define PIN_IMU_SCL 18
  #define PIN_IMU_INT 23
  #define ORIENT_P VERT_F //向き
// 2台目のIMUの設定
  #define PIN_IMU_SDA_2 21
  #define PIN_IMU_SCL_2 22
  #define PIN_IMU_INT_2 25
  #define ORIENT_S VERT_F //向き
#endif

// 角度演算用の行列
#if ORIENT_P == HORIZON
  const float rotation_matrix_P[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
#elif ORIENT_P == VERT_F
  const float rotation_matrix_P[3][3] = {{0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};
#elif ORIENT_P == VERT_B
  const float rotation_matrix_P[3][3] = {{0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}};
#elif ORIENT_P == VERT_L
  const float rotation_matrix_P[3][3] = {{0.0, 0.0, -1.0}, {1.0, 0.0, -1.0}, {0.0, -1.0, 0.0}};
#elif ORIENT_P == VERT_R
  const float rotation_matrix_P[3][3] = {{0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};
#endif
#if ORIENT_S == HORIZON
  const float rotation_matrix_S[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
#elif ORIENT_S == VERT_F
  const float rotation_matrix_S[3][3] = {{0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};
#elif ORIENT_S == VERT_B
  const float rotation_matrix_S[3][3] = {{0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}};
#elif ORIENT_S == VERT_L
  const float rotation_matrix_S[3][3] = {{0.0, 0.0, -1.0}, {1.0, 0.0, -1.0}, {0.0, -1.0, 0.0}};
#elif ORIENT_S == VERT_R
  const float rotation_matrix_S[3][3] = {{0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};
#endif

#endif // DEFINE_H_
