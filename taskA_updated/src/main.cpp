// Assignment1-1275
// Task A - LED ON, OFF, BLINK, FADE
// IOT Fall 2025
// Name : Rabia Naseer              Reg#: 23-NTU-CS-1275

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Pin Configuration ---
#define BUTTON_MODE 26  // D26 for Mode Cycle
#define BUTTON_RESET 25 // D25 for Reset
#define LED1 18         // D18 for LED1
#define LED2 19         // D19 for LED2

// --- LEDC (PWM) Setup ---
const int freq = 5000;          // PWM frequency (Hz)
const int ledChannel1 = 0;      // LEDC channel for LED1
const int ledChannel2 = 1;      // LEDC channel for LED2
const int resolution = 8;       // 8-bit resolution (0–255)
const int MAX_DUTY_CYCLE = 255; // Max duty cycle

// --- Variables ---
int mode = 0;

unsigned long lastDebounce = 0;
const int debounceDelay = 200;

unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 300; // 300 ms blink

unsigned long lastFadeTime = 0;
const unsigned long fadeInterval = 20; // 20 ms fade step

bool ledState = HIGH;
int brightness = 0;
int fadeDirection = 5;

// --- Function Declarations ---
void handleModes(unsigned long currentMillis);
void updateDisplay();

void setup()
{
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);

  // --- LEDC Setup ---
  ledcSetup(ledChannel1, freq, resolution);
  ledcAttachPin(LED1, ledChannel1);

  ledcSetup(ledChannel2, freq, resolution);
  ledcAttachPin(LED2, ledChannel2);

  // --- Display Setup ---
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("System Ready!");
  display.display();

  // Initial state: both LEDs OFF
  ledcWrite(ledChannel1, 0);
  ledcWrite(ledChannel2, 0);
}

void loop()
{
  unsigned long currentMillis = millis();

  // --- Mode Button Handling ---
  if (digitalRead(BUTTON_MODE) == LOW && currentMillis - lastDebounce > debounceDelay)
  {
    mode = (mode + 1) % 4; // Cycle through 4 modes

    // Reset variables for new mode
    ledState = HIGH;
    brightness = 0;
    fadeDirection = 5;

    updateDisplay();

    // Execute immediately for static modes
    if (mode == 0 || mode == 2)
      handleModes(currentMillis);

    lastDebounce = currentMillis;
  }

  // --- Reset Button Handling ---
  if (digitalRead(BUTTON_RESET) == LOW && currentMillis - lastDebounce > debounceDelay)
  {
    mode = 0;
    updateDisplay();

    // Turn both LEDs OFF
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);

    lastDebounce = currentMillis;
  }

  // --- Mode Execution ---
  handleModes(currentMillis);
}

void handleModes(unsigned long currentMillis)
{
  switch (mode)
  {
  case 0: // Both OFF
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);
    break;

  case 1: // Alternate Blink
    if (currentMillis - lastBlinkTime >= blinkInterval)
    {
      lastBlinkTime = currentMillis;

      if (ledState == HIGH)
      {
        ledcWrite(ledChannel1, MAX_DUTY_CYCLE);
        ledcWrite(ledChannel2, 0);
        ledState = LOW;
      }
      else
      {
        ledcWrite(ledChannel1, 0);
        ledcWrite(ledChannel2, MAX_DUTY_CYCLE);
        ledState = HIGH;
      }
    }
    break;

  case 2: // Both ON
    ledcWrite(ledChannel1, MAX_DUTY_CYCLE);
    ledcWrite(ledChannel2, MAX_DUTY_CYCLE);
    break;

  case 3: // PWM Fade
    if (currentMillis - lastFadeTime >= fadeInterval)
    {
      lastFadeTime = currentMillis;
      brightness += fadeDirection;

      // Reverse direction at limits
      if (brightness <= 0 || brightness >= MAX_DUTY_CYCLE)
      {
        fadeDirection = -fadeDirection;
        brightness = constrain(brightness, 0, MAX_DUTY_CYCLE);
      }

      // Opposite fade for two LEDs
      ledcWrite(ledChannel1, brightness);
      ledcWrite(ledChannel2, MAX_DUTY_CYCLE - brightness);
    }
    break;
  }
}

void updateDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Mode: ");

  switch (mode)
  {
  case 0:
    display.println("Both OFF");
    break;
  case 1:
    display.println("Alternate Blink");
    break;
  case 2:
    display.println("Both ON");
    break;
  case 3:
    display.println("PWM Fade");
    break;
  }

  display.display();
}
