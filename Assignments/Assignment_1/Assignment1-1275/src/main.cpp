// Assignment 1
// Embedded IOT Systems Fall 2025

// Name : Rabia Naseer                                          Reg# : 1275

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions (ESP32 GPIO) ---
// Using INPUT_PULLUP, buttons connect to GND.
const int BUTTON_A_PIN = 25; // Cycle/Reset
const int BUTTON_B_PIN = 26; // Press Detection
const int LED1_PIN = 12;     // PWM pin for LED 1
const int LED2_PIN = 13;     // PWM pin for LED 2
const int LED3_PIN = 14;     // Digital pin for LED 3 (Task B)
const int BUZZER_PIN = 27;

// --- OLED Setup (128x64 I2C) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- ESP32 PWM (LEDC) Setup ---
// The ESP32 LEDC (LED Control) peripheral has 16 channels, 0-15.
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8 // 8-bit resolution (0-255)
#define LEDC_CHANNEL_1 0
#define LEDC_CHANNEL_2 1
#define LEDC_CHANNEL_BUZZER 2

// --- Task A: Mode Cycling Variables ---
enum LED_MODE
{
  MODE_OFF = 0,
  MODE_ALTERNATE_BLINK,
  MODE_BOTH_ON,
  MODE_PWM_FADE,
  NUM_MODES
};
LED_MODE currentMode = MODE_OFF;
unsigned long lastModeChange = 0;
unsigned long lastBlinkTime = 0;
const long blinkInterval = 500; // 500 ms
int pwmBrightness = 0;
int pwmFadeDirection = 5; // Step size for fading

// --- Task B: Button Press Detection Variables ---
// Active-Low button logic
#define SHORT_PRESS_MAX_TIME 1500 // Short press is < 1.5s
unsigned long buttonBPressedTime = 0;
bool buttonBPressed = false;
bool led3State = false; // State of LED 3

// --- Debounce Variables ---
unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;
const long debounceDelay = 50;

// Function Prototypes
void updateOLED(const char *modeMsg, const char *eventMsg);
void handleModeCycle(int buttonState);
void resetToOff();
void handleLEDMode();
void handleButtonPressDetection();
void setLED1Brightness(int brightness);
void setLED2Brightness(int brightness);

void setup()
{
  // 1. ESP32 PWM Setup (LEDC)
  ledcSetup(LEDC_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED1_PIN, LEDC_CHANNEL_1);
  ledcSetup(LEDC_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED2_PIN, LEDC_CHANNEL_2);
  ledcSetup(LEDC_CHANNEL_BUZZER, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL_BUZZER);

  // 2. Pin Initialization
  pinMode(LED3_PIN, OUTPUT);
  // Set buttons as input with internal pull-up resistors
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);

  // 3. Initialize Outputs
  resetToOff();
  digitalWrite(LED3_PIN, LOW);

  // 4. Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    // Failed to initialize display
    for (;;)
      ;
  }
  display.display();
  updateOLED("OFF", "System Ready");
}

void loop()
{
  // Read button states (Active-Low: LOW when pressed)
  int buttonAState = digitalRead(BUTTON_A_PIN);
  int buttonBState = digitalRead(BUTTON_B_PIN);
  unsigned long currentTime = millis();

  // --- Task A: Button A (Mode Cycle/Reset) ---
  if (currentTime - lastDebounceTimeA > debounceDelay)
  {
    if (buttonAState == LOW)
    { // Button A is pressed (Active-Low)
      handleModeCycle(buttonAState);
    }
    lastDebounceTimeA = currentTime;
  }

  // --- Task B: Button B (Press Detection) ---
  handleButtonPressDetection();

  // --- Task A: LED Mode Execution ---
  handleLEDMode();
}

/**
 * @brief Sets the brightness of LED 1 using the ESP32 LEDC channel.
 */
void setLED1Brightness(int brightness)
{
  ledcWrite(LEDC_CHANNEL_1, brightness);
}

/**
 * @brief Sets the brightness of LED 2 using the ESP32 LEDC channel.
 */
void setLED2Brightness(int brightness)
{
  ledcWrite(LEDC_CHANNEL_2, brightness);
}

/**
 * @brief Cycles the LED mode on a single button press of Button A.
 */
void handleModeCycle(int buttonState)
{
  if (buttonState == LOW)
  {
    // Debounce prevents rapid cycles in one hold.
    if (millis() - lastModeChange < 500)
      return;

    // Task A: Use Button A to cycle modes
    currentMode = (LED_MODE)((currentMode + 1) % NUM_MODES);

    const char *modeName = "";
    switch (currentMode)
    {
    case MODE_OFF:
      modeName = "BOTH OFF";
      resetToOff();
      break;
    case MODE_ALTERNATE_BLINK:
      modeName = "ALTERNATE BLINK";
      lastBlinkTime = millis();
      setLED1Brightness(255);
      setLED2Brightness(0);
      break;
    case MODE_BOTH_ON:
      modeName = "BOTH ON";
      setLED1Brightness(255);
      setLED2Brightness(255);
      break;
    case MODE_PWM_FADE:
      modeName = "PWM FADE";
      pwmBrightness = 0;
      pwmFadeDirection = 5;
      break;
    }
    lastModeChange = millis();
    updateOLED(modeName, "Button A: Mode Cycle");
  }
}

/**
 * @brief Executes the current LED mode logic. Non-blocking.
 */
void handleLEDMode()
{
  unsigned long currentTime = millis();

  switch (currentMode)
  {
  case MODE_OFF:
    // LEDs are set to 0 in resetToOff()
    break;

  case MODE_ALTERNATE_BLINK:
    if (currentTime - lastBlinkTime >= blinkInterval)
    {
      lastBlinkTime = currentTime;
      // Toggle the state (read/write based on current PWM value)
      if (ledcRead(LEDC_CHANNEL_1) == 255)
      {
        setLED1Brightness(0);
        setLED2Brightness(255);
      }
      else
      {
        setLED1Brightness(255);
        setLED2Brightness(0);
      }
    }
    break;

  case MODE_BOTH_ON:
    // LEDs are constantly HIGH (255)
    break;

  case MODE_PWM_FADE:
    setLED1Brightness(pwmBrightness);
    setLED2Brightness(pwmBrightness);

    pwmBrightness += pwmFadeDirection;

    // Reverse direction at limits (0 to 255)
    if (pwmBrightness <= 0 || pwmBrightness >= 255)
    {
      pwmFadeDirection = -pwmFadeDirection;
      pwmBrightness += pwmFadeDirection;
    }
    // Small pause for visual smoothness
    delay(10);
    break;
  }
}

/**
 * @brief Handles the press-type detection for Button B. Non-blocking.
 */
void handleButtonPressDetection()
{
  unsigned long currentTime = millis();
  int buttonBState = digitalRead(BUTTON_B_PIN);

  // Debouncing logic
  if (currentTime - lastDebounceTimeB > debounceDelay)
  {
    if (buttonBState == LOW)
    { // Button is pressed (Active-Low)
      if (!buttonBPressed)
      {
        // Start of a new press
        buttonBPressedTime = currentTime;
        buttonBPressed = true;
      }

      // Check for Long Press while still holding (allows action before release)
      if (buttonBPressed && (currentTime - buttonBPressedTime >= SHORT_PRESS_MAX_TIME))
      {
        // Task B: Long press (> 1.5 s) -> play buzzer tone
        // Use tone() which works on ESP32 by using LEDC
        // The tone is sustained until released or noTone() is called.
        ledcWriteTone(LEDC_CHANNEL_BUZZER, 1000); // 1000 Hz tone
        updateOLED(currentMode == MODE_OFF ? "BOTH OFF" : "PWM FADE", "Long Press: BUZZER!");
      }
    }
    else
    { // Button is released
      if (buttonBPressed)
      {
        unsigned long pressDuration = currentTime - buttonBPressedTime;

        if (pressDuration < SHORT_PRESS_MAX_TIME)
        {
          // Task B: Short press (<= 1.5 s) -> toggle LED 3
          led3State = !led3State;
          digitalWrite(LED3_PIN, led3State);
          const char *state = led3State ? "ON" : "OFF";
          updateOLED(currentMode == MODE_OFF ? "BOTH OFF" : "PWM FADE", "Short Press: LED3 Toggle");
        }

        // Stop any sustained buzzer tone
        ledcWriteTone(LEDC_CHANNEL_BUZZER, 0); // Stops the tone

        // Reset state for next press
        buttonBPressed = false;
      }
    }
    lastDebounceTimeB = currentTime;
  }
}

/**
 * @brief Clears the screen and displays the current mode and event message.
 */
void updateOLED(const char *modeMsg, const char *eventMsg)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Current Mode/State
  display.setCursor(0, 0);
  display.print("TASK A MODE:");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(modeMsg);

  // Event/Action
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("LAST EVENT:");
  display.setCursor(0, 45);
  display.print(eventMsg);

  // LED 3 Status (Task B indicator)
  display.setCursor(0, 55);
  display.print("LED3 STATUS: ");
  display.print(led3State ? "ON" : "OFF");

  display.display();
}

/**
 * @brief Resets LED1 and LED2 to the OFF state.
 */
void resetToOff()
{
  currentMode = MODE_OFF;
  setLED1Brightness(0);
  setLED2Brightness(0);
}