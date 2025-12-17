// ===============================================================
// Assignment 1 - Task B: Buzzer and LED
// IOT Fall 2025
// Name: Rabia Naseer
// Reg#: 23-NTU-CS-1275
// ===============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------- OLED Configuration -------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ------------------- Pin Configuration --------------------
#define BUTTON_MODE 26 // D26
#define LED1 18        // D18
#define BUZZER_PIN 4   // D4

// ------------------- Button Variables ---------------------
bool buttonPressed = false;
unsigned long pressStartTime = 0;
unsigned long lastDebounce = 0;
const long longPressDuration = 1500; // 1.5 seconds
const int debounceDelay = 50;        // debounce delay (ms)

bool ledState = LOW; // Current LED state

// ------------------- Buzzer Configuration -----------------
#define BUZZER_CHANNEL 0     // LEDC channel
#define BUZZER_RESOLUTION 10 // 10-bit resolution (0–1023)
#define BUZZER_FREQ 1500     // Frequency: 1500 Hz

// ------------------- Function Prototypes ------------------
void handleShortPress();
void handleLongPressHoldDisplay();
void handleLongPressRelease();

// ===============================================================
// Setup
// ===============================================================
void setup()
{
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);

  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  digitalWrite(LED1, ledState);
  ledcWrite(BUZZER_CHANNEL, 0);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Task B Ready");
  display.display();
}

// ===============================================================
// Main Loop
// ===============================================================
void loop()
{
  unsigned long currentMillis = millis();
  int buttonState = digitalRead(BUTTON_MODE);

  if (buttonState == LOW && !buttonPressed && currentMillis - lastDebounce > debounceDelay)
  {
    buttonPressed = true;
    pressStartTime = currentMillis;
    lastDebounce = currentMillis;
  }
  else if (buttonState == HIGH && buttonPressed)
  {
    long pressDuration = currentMillis - pressStartTime;

    if (pressDuration > 50)
    {
      if (pressDuration >= longPressDuration)
      {
        handleLongPressRelease();
      }
      else
      {
        handleShortPress();
      }
    }

    buttonPressed = false;
    ledcWrite(BUZZER_CHANNEL, 0);
    lastDebounce = currentMillis;
  }

  if (buttonPressed && (currentMillis - pressStartTime >= longPressDuration))
  {
    ledcWrite(BUZZER_CHANNEL, (1 << BUZZER_RESOLUTION) / 2);

    if (currentMillis - lastDebounce > 1000)
    {
      handleLongPressHoldDisplay();
      lastDebounce = currentMillis;
    }
  }
  else if (!buttonPressed && currentMillis - lastDebounce > 5000)
  {
    // Optional idle display
  }
}

// ===============================================================
// Helper Functions
// ===============================================================
void handleShortPress()
{
  ledState = !ledState;
  digitalWrite(LED1, ledState);

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Short Press:");
  display.setCursor(0, 20);
  display.print("LED ");
  display.println(ledState ? "ON" : "OFF");
  display.display();
}

void handleLongPressHoldDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Long Press Hold!");
  display.setCursor(0, 20);
  display.println("BUZZER ON (1.5s+)");
  display.display();
}

void handleLongPressRelease()
{
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Long Press Ended.");
  display.setCursor(0, 20);
  display.println("Buzzer stopped.");
  display.display();
}
