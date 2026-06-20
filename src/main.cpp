#include <Arduino.h>
#include <BleKeyboard.h>

// ── Pins ──────────────────────────────────────────────────────────────────
#define PIN_CLK  18   // Rotary encoder CLK (A)
#define PIN_DT   19   // Rotary encoder DT  (B)
#define PIN_SW    5   // Rotary encoder push button

// ── Timing ────────────────────────────────────────────────────────────────
#define ENCODER_DEBOUNCE_MS   4
#define BUTTON_DEBOUNCE_MS   50

BleKeyboard bleKeyboard("Volume Controller", "DIY", 100);

static int           lastClk          = HIGH;
static unsigned long lastEncoderTime  = 0;
static unsigned long lastButtonTime   = 0;
static bool          buttonWasPressed = false;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT,  INPUT_PULLUP);
  pinMode(PIN_SW,  INPUT_PULLUP);

  lastClk = digitalRead(PIN_CLK);

  bleKeyboard.begin();
  Serial.println("BLE keyboard started — pair \"Volume Controller\" on your device");
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    delay(100);
    return;
  }

  // ── Rotary encoder ──────────────────────────────────────────────────────
  int clk = digitalRead(PIN_CLK);

  if (clk != lastClk && millis() - lastEncoderTime > ENCODER_DEBOUNCE_MS) {
    lastEncoderTime = millis();

    if (clk == LOW) {
      if (digitalRead(PIN_DT) == HIGH) {
        bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        Serial.println("Volume +");
      } else {
        bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        Serial.println("Volume -");
      }
    }
    lastClk = clk;
  }

  // ── Button (play / pause) ───────────────────────────────────────────────
  bool buttonPressed = digitalRead(PIN_SW) == LOW;

  if (buttonPressed && !buttonWasPressed &&
      millis() - lastButtonTime > BUTTON_DEBOUNCE_MS) {
    lastButtonTime   = millis();
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    Serial.println("Play/Pause");
  }
  buttonWasPressed = buttonPressed;
}
