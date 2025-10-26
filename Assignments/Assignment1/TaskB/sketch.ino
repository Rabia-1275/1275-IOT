// Assignment1-1275
// Task B - Buzzer and LED
// IOT Fall 2025
// Name : Rabia Naseer                                Reg#-23-NTU-CS-1275


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin Configuration (Matches the final diagram.json)
#define BUTTON_MODE 26  // D26
#define LED1 18         // D18
#define BUZZER_PIN 4    // D4

// --- Button Logic Variables ---
bool buttonPressed = false;
unsigned long pressStartTime = 0;
const long longPressDuration = 1500; // 1.5 seconds threshold
const int debounceDelay = 50;        
unsigned long lastDebounce = 0;

bool ledState = LOW; // Current state of the LED

void setup() {
    // Pin Modes Setup
    pinMode(BUTTON_MODE, INPUT_PULLUP); // Button is wired to GND, so use INPUT_PULLUP
    pinMode(LED1, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Initial State
    digitalWrite(LED1, ledState); 
    noTone(BUZZER_PIN); 

    // Initialize OLED 
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Task B Ready");
    display.display();
}

void loop() {
    unsigned long currentMillis = millis();
    int buttonState = digitalRead(BUTTON_MODE); 

    // 1. --- Button Press Detected (Start Timing) ---
    // If button is LOW and we are not currently tracking a press
    if (buttonState == LOW && !buttonPressed && currentMillis - lastDebounce > debounceDelay) {
        buttonPressed = true;
        pressStartTime = currentMillis;
        lastDebounce = currentMillis;
    }

    // 2. --- Button Released (Check Duration and Execute Action) ---
    // If button is HIGH and we were tracking a press
    else if (buttonState == HIGH && buttonPressed) {
        
        long pressDuration = currentMillis - pressStartTime;
        
        // This check handles the action on release.
        if (pressDuration > 50) { // Ensure a minimum press time (not just electrical noise)
            
            // A. Long Press Action (> 1.5s)
            if (pressDuration >= longPressDuration) {
                // Action: Long press -> handled by tone/noTone in loop (3 & 4)
                handleLongPressRelease();
            } 
            
            // B. Short Press Action (< 1.5s)
            else {
                // Action: Short press -> toggle LED
                handleShortPress();
            }
        }
        
        // Reset state and debounce timer
        buttonPressed = false;
        noTone(BUZZER_PIN); // Ensure tone stops immediately on release
        lastDebounce = currentMillis;
    }
    
    // 3. --- Sustained Long Press Action (Play Buzzer) ---
    // If button is held down, and the long press time has been exceeded
    if (buttonPressed && (currentMillis - pressStartTime >= longPressDuration)) {
         tone(BUZZER_PIN, 1500); // Play a loud 1500 Hz tone
         
         // Update display while holding (only update once per second to avoid flicker)
         if (currentMillis - lastDebounce > 1000) { 
             handleLongPressHoldDisplay(); 
             lastDebounce = currentMillis;
         }
    }
    // 4. --- Pre-Press State (Optional, ensures display shows ready state) ---
    else if (!buttonPressed && currentMillis - lastDebounce > 5000) {
        // Optional: Ensure the screen doesn't go blank if user waits too long
        // You can add a periodic "Ready" display here if needed.
    }
}

void handleShortPress() {
    // 1. Toggle LED
    ledState = !ledState;
    digitalWrite(LED1, ledState);

    // 2. Update Display
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Short Press:");
    display.setCursor(0, 20);
    display.print("LED ");
    display.println(ledState ? "ON" : "OFF");
    display.display();
}

void handleLongPressHoldDisplay() {
    // Runs repeatedly while the button is HELD for a long press.
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Long Press Hold!");
    display.setCursor(0, 20);
    display.println("BUZZER ON (1.5s+)");
    display.display();
}

void handleLongPressRelease() {
    // Runs once when the button is released AFTER a long press.
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Long Press Ended.");
    display.setCursor(0, 20);
    display.println("Buzzer stopped.");
    display.display();
}
