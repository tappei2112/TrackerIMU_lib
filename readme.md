# USBシリアル通信式慣性モーショントラッカー ファームウェア

このプログラムは、ESP32と6~9軸のIMUセンサーを利用し、モーションをトラッキングするためのファームウェアです。
現在は、BMI160を使用する場合のコードのみ開発しております。

## 開発環境
- Platform IO (VSCode)
- [ESP32-DevKitC-32E](https://akizukidenshi.com/catalog/g/g115673/)
- [中華のBMI160モジュール](https://www.amazon.co.jp/gp/product/B083R3PTJ5/)

## 使用ライブラリ
- [EmotiBit BMI160](https://github.com/EmotiBit/EmotiBit_BMI160)

## 注意事項
- センサーのフルスケール値を変更するため、ライブラリのコードを一部書き換えます
  
	EmotiBit_BMI160ライブラリにあるBMI160.cppのL116～L117を書き換える
  setFullScaleGyroRange(BMI160_GYRO_RANGE_1000);
  setFullScaleAccelRange(BMI160_ACCEL_RANGE_8G);



## 回路

※INTピンはつながなくてよい(現在のプログラムにおいて、初期化時に使うため一応用意している)

プライマリIMU(BMI160 0x68)
| BMI160 PIN | ESP32 PIN |
| ---------- | --------- |
|     3V3    |    3V3    |
|     GND    |    GND    |
|     SCL    |   GPIO22  |
|     SDA    |   GPIO21  |
|     SDO    |    GND    |
|     INT    |   GPIO23  |

セカンダリIMU(BMI160 0x69)
| BMI160 PIN | ESP32 PIN |
| ---------- | --------- |
|     3V3    |    3V3    |
|     GND    |    GND    |
|     SCL    |   GPIO18  |
|     SDA    |   GPIO19  |
|     INT    |   GPIO25  |
