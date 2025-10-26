// Assignment1-1275
// Task A - LED ON,OFF,BLINK,FADE
// IOT Fall 2025
// Name : Rabia Naseer                            Reg#:23-NTU-CS-1275


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin configuration -
#define BUTTON_MODE 26  // D26 for Mode Cycle
#define BUTTON_RESET 25 // D25 for Reset
#define LED1 18         // D18 for LED1
#define LED2 19         // D19 for LED2

int mode = 0;

unsigned long lastDebounce = 0;
const int debounceDelay = 200;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 300; // 300ms for blink
unsigned long lastFadeTime = 0;
const unsigned long fadeInterval = 20; // 20ms for fade step

// Variables for Alternate Blink mode
bool ledState = HIGH;

// Variables for PWM Fade mode
int brightness = 0;
int fadeDirection = 5; 


void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("System Ready!");
  display.display();

  analogWrite(LED1, 0); // Use analogWrite(pin, 0) for OFF
  analogWrite(LED2, 0);
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Button Handling (Mode Cycle) ---
  if (digitalRead(BUTTON_MODE) == LOW && currentMillis - lastDebounce > debounceDelay) {
    mode = (mode + 1) % 4;
    
    // Reset state variables for new mode
    ledState = HIGH;
    brightness = 0;
    fadeDirection = 5;
    
    updateDisplay();
    // Execute mode logic immediately for non-timing modes (0 and 2)
    if (mode == 0 || mode == 2) {
      handleModes(currentMillis);
    }
    lastDebounce = currentMillis;
  }

  // --- Button Handling (Reset to OFF) ---
  if (digitalRead(BUTTON_RESET) == LOW && currentMillis - lastDebounce > debounceDelay) {
    mode = 0;
    updateDisplay();
    // Turn LEDs off immediately
    analogWrite(LED1, 0);
    analogWrite(LED2, 0);
    
    lastDebounce = currentMillis;
  }
  
  // --- Mode Execution ---
  handleModes(currentMillis);
}

void handleModes(unsigned long currentMillis) {
  switch (mode) {
    case 0: // Both OFF
      analogWrite(LED1, 0);
      analogWrite(LED2, 0);
      break;

    case 1: // Alternate Blink
      if (currentMillis - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = currentMillis;

        if (ledState == HIGH) {
          analogWrite(LED1, 255); // Full ON (255)
          analogWrite(LED2, 0);    // OFF (0)
          ledState = LOW; 
        } else {
          analogWrite(LED1, 0);  // OFF (0)
          analogWrite(LED2, 255); // Full ON (255)
          ledState = HIGH; 
        }
      }
      break;

    case 2: // Both ON
      analogWrite(LED1, 255); // Full ON
      analogWrite(LED2, 255); // Full ON
      break;

    case 3: // PWM Fade
      if (currentMillis - lastFadeTime >= fadeInterval) {
        lastFadeTime = currentMillis;

        brightness += fadeDirection;

        // Change direction at the limits (0 and 255)
        if (brightness <= 0 || brightness >= 255) {
          fadeDirection = -fadeDirection; // Reverse direction
          if (brightness < 0) brightness = 0;
          if (brightness > 255) brightness = 255;
        }

        // Use analogWrite(pin, value) for fading
        analogWrite(LED1, brightness);
        analogWrite(LED2, 255 - brightness); // Inverse fade
      }
      break;
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Mode: ");
  switch (mode) {
    case 0: display.println("Both OFF"); break;
    case 1: display.println("Alternate Blink"); break;
    case 2: display.println("Both ON"); break;
    case 3: display.println("PWM Fade"); break;
  }
  display.display();
}