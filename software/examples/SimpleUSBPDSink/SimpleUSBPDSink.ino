#include <usbpd_def.h>
#include <usbpd_sink.h>

void setup() {
  // USB PDのシンクデバイスとして初期化
  usbpd_sink_init();
    
  // ディレイを入れないとうまく通信できず、USB PD電圧が出力できない場合がある
  delay(1000);
    
  // USB PDシンクとして準備完了なら20Vを電源にリクエスト
  if (usbpd_sink_get_ready()) {
    usbpd_sink_set_request_fixed_voltage(REQUEST_20v);
  }
}

void loop() {
  // ここでは何も行わない
}