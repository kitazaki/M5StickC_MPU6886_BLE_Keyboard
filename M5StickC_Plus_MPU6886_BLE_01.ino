// M5StickC Plus MPU6886
// https://pages.switch-science.com/letsiot/vibration/
// Bluetooth keyboard
// https://programresource.net/2020/04/09/3244.html
//
#include <M5StickCPlus.h>
#include <BleKeyboard.h>  // BLE keyboard
 
BleKeyboard bleKeyboard("M5StickCPlus");

#define SAMPLE_PERIOD 20    // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 150     // 20ms x 150 = 3秒

void setup() {
    Serial.begin(115200);
    M5.begin();
    bleKeyboard.begin();
    M5.Lcd.setRotation(3);
    M5.Imu.Init();  // MPU6886を初期設定する
}

float ax, ay, az[SAMPLE_SIZE];  // 加速度データを読み出す変数

#define X0 5  // 横軸の描画開始座標
#define MINZ 500  // 縦軸の最小値 600mG
#define MAXZ 1500  // 縦軸の最大値 1400mG

#define TIME 25  // 20ms x 25 = 0.5秒
int count;  // チャタリング防止用カウンタ　(JUMP検知でcount = 0、TIMEを経過するまでは次のJUMPを行わない)

void loop() {
    M5.Lcd.fillScreen(BLACK);  // 画面をクリア
    M5.Lcd.setCursor(0, 5);  // テキスト表示位置をクリア

    if (bleKeyboard.isConnected()) {
      M5.Lcd.println("Connected");
    }
    else {
      M5.Lcd.println("Disconnected");
    }  

    for (int i = 0; i < SAMPLE_SIZE; i++) {
//        M5.MPU6886.getAccelData(&ax,&ay,&az[i]);  // MPU6886から加速度を取得
        M5.Imu.getAccelData(&ax,&az[i],&ay);  // Z軸とY軸を入れ替え

        az[i] *= 1000;  // mGに変換
        ax *= 1000;
        ay *= 1000;
        
        if (i == 0) continue;
        int y0 = map((int)(az[i - 1]), MINZ, MAXZ, M5.Lcd.height(), 0);
        int y1 = map((int)(az[i]), MINZ, MAXZ, M5.Lcd.height(), 0);
        M5.Lcd.drawLine(i - 1 + X0, y0, i + X0, y1, GREEN);
        //Serial.printf("%7.2f,%7.2f,%7.2f\n", ax * M5.MPU6886.gRes,  ay * M5.MPU6886.gRes, az[i] * M5.MPU6886.gRes);

        if (az[i] > 1500) {
          Serial.printf("az: %7.2f\n", az[i]);
          if (count > TIME) {
            M5.Lcd.println("JUMP!");
            Serial.println("JUMP!");
            bleKeyboard.print(" ");  // スペースキー
            count = 0;  // カウンタリセット
          }
        }
        count++;

        delay(SAMPLE_PERIOD);
    }
}
