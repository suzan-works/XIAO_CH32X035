// 初期ファーム
// 5秒ごとにPD電圧を切り替える、PA4アナログ値をシリアル送信する

#include <usbpd_def.h>
#include <usbpd_sink.h>

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
const int pin = PA4; // VBUS = PA4 x 11

int pdIndex = 0;
int setVoltage = REQUEST_5v;

void setup()
{
  Serial.begin(115200);
  pinMode(pin, INPUT);
  usbpd_sink_init();
  delay(1000);
}

void loop()
{
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
      setVoltage = REQUEST_5v;
      break;
  }
  Serial.println(pdIndex);

  if(usbpd_sink_get_ready())
  {
    usbpd_sink_set_request_fixed_voltage(setVoltage);
  }
  delay(1000);

  int value = analogRead(pin);
  Serial.println(value);
  delay(4000);

  pdIndex = (pdIndex + 1) % 5;

}