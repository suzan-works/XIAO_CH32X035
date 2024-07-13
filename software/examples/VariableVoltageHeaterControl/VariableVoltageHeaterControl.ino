#include <usbpd_def.h>
#include <usbpd_sink.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

uint8_t setVoltage = REQUEST_5v;  // 初期設定のリクエスト電圧は5V

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAXDO   PA6
#define MAXCS   PB11 // RX
#define MAXCLK  PA5
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

const int vbusControlPin = PA0;
const int divideVoltPin = PA4;

float volt = 5;
float temp = 25;
int degCDisplay = 25;
bool outputState = 0;
double degC = 25;
const double tempA = 60;
const double tempB = 50;

void setup() {
  // Serial.begin(115200);

  // USB PDシンクの初期化
  usbpd_sink_init();
  
  // OLEDディスプレイの初期化と起動表示
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 35);
  display.println("XIAO");
  display.setCursor(35, 50);
  display.println("CH32X035");
  display.display();

  pinMode(vbusControlPin, OUTPUT); // VBUS制御ピンを出力に設定
  pinMode(divideVoltPin, INPUT);   // 分圧ピンを入力に設定

  // 熱電対アンプモジュールの初期化
  if (!thermocouple.begin()) {
    // Serial.println("ERROR.");
    while (1) delay(10);
  }

  delay(2000);
}

void loop() {
  // ヒーター温度の読み取り
  degC = thermocouple.readCelsius();
  if (isnan(degC)) {
    uint8_t e = thermocouple.readError();
  }
  if ((degC > 10)&&(degC <100)) {
    degCDisplay = (int) degC;
  }

  // VBUS電圧の読み取り
  int voltRead = analogRead(divideVoltPin);
  float voltCalc = (float) voltRead;
  voltCalc = ((voltCalc * 11 * 3.3) / 4095);  // 読み取った値を実際の電圧に変換
  volt = voltCalc;

  // ヒーター温度から動作を判断
  if (degC >= tempA) {
    outputState = 0;
  } else {
    if (degC >= tempB) {
      setVoltage = REQUEST_9v;
    } else {
      setVoltage = REQUEST_15v;
    }
    // USB PDシンクが準備完了か確認し、リクエスト電圧を設定
    if (usbpd_sink_get_ready()) {
      usbpd_sink_set_request_fixed_voltage(setVoltage);
    }
    delay(500);
    if (setVoltage == REQUEST_9v) {
      if ((volt > 8)&&(volt < 10)) {
        outputState = 1;
      } else {
        outputState = 0;
      }
    }
    if (setVoltage == REQUEST_15v) {
      if ((volt > 14)&&(volt < 16)) {
        outputState = 1;
      } else {
        outputState = 0;
      }
    }
  }
  digitalWrite(vbusControlPin, outputState);

  // OLEDディスプレイの表示
  display.clearDisplay();
  display.setCursor(35, 35);
  display.print(volt);
  display.println("V");
  display.setCursor(35, 50);
  display.print(degCDisplay);
  display.println("C");
  if (outputState) {
    display.setCursor(64, 50);
    display.println("ON");
  } else {
    display.setCursor(64, 50);
    display.println("OFF");
  }
  display.display();

  delay(500);
}

