#include <FastLED.h>

// 定义灯带的相关参数
#define LED_PIN     6
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

// 定义麦克风输入引脚
#define MIC_PIN A0

// 颜色变化模式
#define MODE_SINGLE_COLOR 0
#define MODE_RAINBOW 1
#define MODE_FADE 2
#define MODE_PULSE 3
int currentMode = MODE_SINGLE_COLOR;

// 单颜色模式下的颜色
CRGB singleColor = CRGB::Red;

// 彩虹模式参数
uint8_t rainbowHue = 0;

// 渐变模式参数
CRGB fadeColor = CRGB::Blue;
int fadeDirection = 1;

// 脉冲模式参数
int pulseBrightness = 0;
int pulseDirection = 1;

// 声音阈值
const int soundThreshold = 50;

void setup() {
  // 初始化串口通信
  Serial.begin(9600);

  // 初始化灯带
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
}

void loop() {
  // 读取麦克风数据
  int soundLevel = analogRead(MIC_PIN);

  // 根据声音强度控制灯带亮度
  int brightness = map(soundLevel, 0, 1023, 0, 255);
  if (brightness < 0) brightness = 0;
  if (brightness > 255) brightness = 255;

  // 根据不同模式更新灯带颜色
  switch (currentMode) {
    case MODE_SINGLE_COLOR:
      singleColorMode(brightness);
      break;
    case MODE_RAINBOW:
      rainbowMode(brightness);
      break;
    case MODE_FADE:
      fadeMode(brightness);
      break;
    case MODE_PULSE:
      pulseMode(brightness);
      break;
  }

  // 显示灯带颜色
  FastLED.show();

  // 切换模式
  if (Serial.available()) {
    char command = Serial.read();
    if (command == '1') currentMode = MODE_SINGLE_COLOR;
    else if (command == '2') currentMode = MODE_RAINBOW;
    else if (command == '3') currentMode = MODE_FADE;
    else if (command == '4') currentMode = MODE_PULSE;
  }

  // 稍微延迟一下
  delay(10);
}

void singleColorMode(int brightness) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = singleColor;
    leds[i].setBrightness(brightness);
  }
}

void rainbowMode(int brightness) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(rainbowHue + i * 10, 255, brightness);
  }
  rainbowHue++;
}

void fadeMode(int brightness) {
  if (fadeColor.r >= 255) fadeDirection = -1;
  if (fadeColor.r <= 0) fadeDirection = 1;
  fadeColor.r += fadeDirection;
  fadeColor.g += fadeDirection;
  fadeColor.b += fadeDirection;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = fadeColor;
    leds[i].setBrightness(brightness);
  }
}

void pulseMode(int brightness) {
  if (pulseBrightness >= 255) pulseDirection = -1;
  if (pulseBrightness <= 0) pulseDirection = 1;
  pulseBrightness += pulseDirection;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
    leds[i].setBrightness(pulseBrightness * brightness / 255);
  }
}
