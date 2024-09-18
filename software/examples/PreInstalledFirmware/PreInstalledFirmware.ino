// ファームウェア情報
// バージョン: 1.0
// ファイル名: PreInstalledFirmware.ino
// 対象ボード: CH32X035搭載XIAOピン互換ボード (CH32X035 XIAO-Compatible Board)
// 設計者: GitHubアカウント suzan-works , Xアカウント @suzan_works

// 動作概要
// 5秒ごとにUSB PD（Power Delivery）の電圧を切り替えます。
// PA4ピンからアナログ電圧を読み取り、その値をシリアル通信で送信します。

// 参考
// https://github.com/openwch/arduino_core_ch32/tree/main/libraries/USBPD_SINK

#include <usbpd_def.h>
#include <usbpd_sink.h>

// ピンアサイン
//const int pin = PA0; // "A0"
//const int pin = PA1; // "A1"
//const int pin = PA2; // "A2"
//const int pin = PA3; // "A3"
//const int pin = PC17; // "SDA"
//const int pin = PC16; // "SCL"
//const int pin = PB10; // "TX"
//const int pin = PB11; // "RX"
//const int pin = PA5; // "SCK"
//const int pin = PA6; // "MI"
//const int pin = PA7; // "MO"

// アナログ入力ピンの定義（VBUS電圧の読み取り用）
// VBUSは電圧分圧回路（10k:1k）を介してPA4に接続されています。 VBUS = PA4 x 11
const int pin = PA4;

// PD電圧要求を管理するための変数
int pdIndex = 0;
int setVoltage = REQUEST_5v;

void setup()
{
  Serial.begin(115200);
  pinMode(pin, INPUT);
  usbpd_sink_init(); // USB PDシンク機能を初期化
  delay(1000);
}

void loop()
{
  // 現在のpdIndexに基づいて要求する電圧を設定
  switch(pdIndex)
  {
    case 0:
      setVoltage = REQUEST_5v;
      break;
    case 1:
      setVoltage = REQUEST_9v;
      break;
    case 2:
      setVoltage = REQUEST_12v;
      break;
    case 3:
      setVoltage = REQUEST_15v;
      break;
    case 4:
      setVoltage = REQUEST_20v;
      break;
    default:
      setVoltage = REQUEST_5v; // デフォルトで5Vを要求
      break;
  }
  Serial.println(pdIndex); // 現在のpdIndexをシリアルモニタに出力

  // USB PDシンクが準備完了か確認してから新しい電圧を要求
  if(usbpd_sink_get_ready())
  {
    usbpd_sink_set_request_fixed_voltage(setVoltage);
  }
  delay(1000); // 電圧変更の反映を待つために1秒待機

  // PA4からアナログ値を読み取り
  int value = analogRead(pin);
  Serial.println(value);
  delay(4000); // 次の電圧変更まで4秒待機（合計で5秒間隔）

  // pdIndexをインクリメントし、0〜4の範囲でループ
  pdIndex = (pdIndex + 1) % 5;
}
