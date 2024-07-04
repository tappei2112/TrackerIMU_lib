#include "filterIMU.h"
#include <math.h>

float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
float beta = 0.1f;
float sampleFreq = 512.0f;

// ローパスフィルタ関数
float lowPassFilter(float input, float previous, float alpha) {
  return previous + alpha * (input - previous);
}

// ハイパスフィルタ関数
float highPassFilter(float input, float previousInput, float previousOutput, float alpha) {
  return alpha * (previousOutput + input - previousInput);
}

// バンドパスフィルタ関数
float bandPassFilter(float input, float *previousLowPass, float *previousHighPass, float alphaLowPass, float alphaHighPass) {
  // ローパスフィルタを適用
  float lowPassOutput = lowPassFilter(input, *previousLowPass, alphaLowPass);

  // ローパスフィルタを適用した値にハイパスフィルタを適用
  float highPassOutput = highPassFilter(lowPassOutput, *previousLowPass, *previousHighPass, alphaHighPass);

	// 前回値の更新
	*previousLowPass = lowPassOutput;
	*previousHighPass = highPassOutput;

  return highPassOutput;
}

void madgwickUpdate(float *gyro, float *accel, float *angle) {
  float gx = gyro[0];
  float gy = gyro[1];
  float gz = gyro[2];
  float ax = accel[0];
  float ay = accel[1];
  float az = accel[2];

	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];  // 現在の四元数
  float norm;
  float s1, s2, s3, s4;
  float hx, hy, bx, bz;
  float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
  float halfex, halfey, halfez;
  float qa, qb, qc;

  // 加速度センサーのデータを正規化
  norm = sqrt(ax * ax + ay * ay + az * az);
  if (norm == 0.0f) return;  // データが無効な場合、更新しない
  norm = 1.0f / norm;  // 正規化
  ax *= norm;
  ay *= norm;
  az *= norm;

  // フィルタの誤差計算
  halfvx = q2 * q4 - q1 * q3;
  halfvy = q1 * q2 + q3 * q4;
  halfvz = q1 * q1 - 0.5f + q4 * q4;

  halfex = (ay * halfvz - az * halfvy);
  halfey = (az * halfvx - ax * halfvz);
  halfez = (ax * halfvy - ay * halfvx);

  // フィードバック
  if (beta > 0.0f) {
    s1 = -2.0f * (q3 * halfey - q4 * halfez);
    s2 = 2.0f * (q4 * halfex + q2 * halfez);
    s3 = 2.0f * (q2 * halfey - q1 * halfex);
    s4 = -2.0f * (q1 * halfez + q3 * halfex);

    norm = 1.0f / sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    q1 -= beta * s1;
    q2 -= beta * s2;
    q3 -= beta * s3;
    q4 -= beta * s4;
  }

  // 角速度データの統合
  gx *= (0.5f * (1.0f / sampleFreq));
  gy *= (0.5f * (1.0f / sampleFreq));
  gz *= (0.5f * (1.0f / sampleFreq));
  qa = q1;
  qb = q2;
  qc = q3;
  q1 += (-qb * gx - qc * gy - q4 * gz);
  q2 += (qa * gx + qc * gz - q4 * gy);
  q3 += (qa * gy - qb * gz + q4 * gx);
  q4 += (qa * gz + qb * gy - qc * gx);

  // 四元数の正規化
  norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
  norm = 1.0f / norm;
  q[0] = q1 * norm;
  q[1] = q2 * norm;
  q[2] = q3 * norm;
  q[3] = q4 * norm;

	quaternionToEuler(q, angle);
}

// 四元数からピッチ、ロール、ヨーを計算する関数
void quaternionToEuler(float *q, float *angle) {
  // 四元数成分
  float qw = q[0];
  float qx = q[1];
  float qy = q[2];
  float qz = q[3];

  // ピッチ（x軸周りの回転）
  float sinr_cosp = 2.0f * (qw * qx + qy * qz);
  float cosr_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
  angle[1] = atan2(sinr_cosp, cosr_cosp) * 180.0 / M_PI;

  // ヨー（z軸周りの回転）
  float sinp = 2.0f * (qw * qy - qz * qx);
  if (abs(sinp) >= 1.0f)
    angle[0] = copysign(M_PI / 2.0f, sinp);  // ピッチは90度
  else
    angle[0] = asin(sinp) * 180.0 / M_PI;

  // ロール（y軸周りの回転）
  float siny_cosp = 2.0f * (qw * qz + qx * qy);
  float cosy_cosp = 1.0f - 2.0f * (qy * qy + qz * qz);
  angle[2] = atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
}
