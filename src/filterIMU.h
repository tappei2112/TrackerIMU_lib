#ifndef FILTERIMU_H_
#define FILTERIMU_H_

// ローパスフィルタ関数
float lowPassFilter(float input, float previous, float alpha);

// ハイパスフィルタ関数
float highPassFilter(float input, float previousInput, float previousOutput, float alpha);

// バンドパスフィルタ関数
float bandPassFilter(float input, float *previousLowPass, float *previousHighPass, float alphaLowPass, float alphaHighPass);

// Madgwickフィルタ関数
void madgwickUpdate(float *gyro, float *accel, float *angle);

// 四元数をオイラー角に変換する関数
void quaternionToEuler(float *q, float *angle);

#endif // FILTER_H_
