// /****************************************************
//  * ESP32 + DHT22 + SSD1306 OLED + Button + Blynk
//  * Pins (from Wokwi diagram):
//  *   DHT22 data -> GPIO23
//  *   OLED SDA   -> GPIO21
//  *   OLED SCL   -> GPIO22
//  *   Button     -> GPIO5 (active LOW, uses INPUT_PULLUP)
//  ****************************************************/

// #define BLYNK_TEMPLATE_ID "TMPL6NH14QjBS"
// #define BLYNK_TEMPLATE_NAME "DHT by Rabia"
// #define BLYNK_AUTH_TOKEN "zhUg9k7vrJ5Z3e_WtOuKo5ojk89hrht4"

// #define BLYNK_PRINT Serial

// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <BlynkSimpleEsp32.h>

// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #include "DHT.h"

// // ------------ WiFi credentials (for wokwi) ------------

// char ssid[] = "Wokwi-GUEST";
// char password[] = "";

// // ------------ Pins (match your Wokwi diagram) ------------
// #define DHTPIN 23
// #define DHTTYPE DHT11

// #define BUTTON_PIN 5 // pushbutton, one side to GPIO5, other to GND

// // ------------ OLED settings ------------
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_RESET -1 // no reset pin

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// DHT dht(DHTPIN, DHTTYPE);

// BlynkTimer timer;

// // For simple button edge detection
// int lastButtonState = HIGH;

// // Forward declaration
// void readAndDisplayAndSend();

// // Optional: send periodically to Blynk (even without button)
// void periodicSend()
// {
//   readAndDisplayAndSend();
// }

// void setup()
// {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println();
//   Serial.println("ESP32 DHT22 + OLED + Blynk starting...");

//   // Button input with internal pull-up
//   pinMode(BUTTON_PIN, INPUT_PULLUP);

//   // I2C + OLED
//   Wire.begin(21, 22); // SDA, SCL (as per diagram)
//   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
//   {
//     Serial.println("SSD1306 allocation failed");
//     for (;;)
//       ; // halt if OLED not found
//   }
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);
//   display.println("Booting...");
//   display.display();

//   // DHT sensor
//   dht.begin();

//   // Blynk (for real hardware WiFi)
//   Serial.println("Connecting to Blynk...");
//   Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
//   // For Wokwi, WiFi is simulated via wokwi.toml [net] config

//   // Periodic send every 5 seconds (optional)
//   timer.setInterval(5000L, periodicSend);
// }

// // Reads DHT22, updates OLED and sends to Blynk
// void readAndDisplayAndSend()
// {
//   float h = dht.readHumidity();
//   float t = dht.readTemperature(); // Celsius

//   if (isnan(h) || isnan(t))
//   {
//     Serial.println("Failed to read from DHT sensor!");
//     display.clearDisplay();
//     display.setCursor(0, 0);
//     display.println("DHT Error!");
//     display.display();
//     return;
//   }

//   Serial.print("Temp: ");
//   Serial.print(t);
//   Serial.print(" *C, Hum: ");
//   Serial.print(h);
//   Serial.println(" %");

//   // --- Update OLED ---
//   display.clearDisplay();
//   display.setCursor(0, 0);
//   display.println("Environment Node");
//   display.println("-----------------");
//   display.print("Temp: ");
//   display.print(t, 1);
//   display.println(" C");
//   display.print("Hum : ");
//   display.print(h, 1);
//   display.println(" %");
//   display.println();
//   display.println("BTN -> manual update");
//   display.display();

//   // --- Send to Blynk (Virtual Pins) ---
//   // Map: V0 = Temp, V1 = Humidity
//   Blynk.virtualWrite(V0, t);
//   Blynk.virtualWrite(V1, h);
// }

// void loop()
// {
//   Blynk.run();
//   timer.run();

//   // Simple button edge detection (active LOW)
//   int currentState = digitalRead(BUTTON_PIN);
//   if (lastButtonState == HIGH && currentState == LOW)
//   {
//     // Falling edge -> button pressed
//     Serial.println("Button pressed: manual DHT read");
//     readAndDisplayAndSend();
//   }
//   lastButtonState = currentState;
// }

/****************************************************
 * ESP32 + DHT22 + MQTT (PUBLISHER ONLY)
 * Topic:
 *   home/lab1/temp
 *   home/lab1/hum
 ****************************************************/

// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "DHT.h"

// // ---------- WiFi ----------
// char ssid[] = "Wokwi-GUEST";
// char pass[] = "";

// // ---------- MQTT ----------
// const char *mqtt_server = "10.13.47.80"; // Mosquitto / Cloud broker
// const int mqtt_port = 1883;

// // Topics
// const char *TOPIC_TEMP = "home/lab1/temp";
// const char *TOPIC_HUM = "home/lab1/hum";

// // ---------- DHT ----------
// #define DHTPIN 23
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// // ---------- MQTT Client ----------
// WiFiClient espClient;
// PubSubClient mqtt(espClient);

// // ---------- Functions ----------
// void connectWiFi()
// {
//   Serial.print("Connecting to WiFi");
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");
// }

// void connectMQTT()
// {
//   while (!mqtt.connected())
//   {
//     Serial.print("Connecting to MQTT...");
//     if (mqtt.connect("ESP32_Publisher"))
//     {
//       Serial.println("connected");
//     }
//     else
//     {
//       Serial.print("failed, rc=");
//       Serial.println(mqtt.state());
//       delay(2000);
//     }
//   }
// }

// void setup()
// {
//   Serial.begin(115200);

//   dht.begin();
//   connectWiFi();

//   mqtt.setServer(mqtt_server, mqtt_port);
//   connectMQTT();
// }

// void loop()
// {
//   if (!mqtt.connected())
//   {
//     connectMQTT();
//   }
//   mqtt.loop();

//   float temperature = dht.readTemperature();
//   float humidity = dht.readHumidity();

//   if (isnan(temperature) || isnan(humidity))
//   {
//     Serial.println("DHT read failed");
//     delay(2000);
//     return;
//   }

//   char tBuf[8], hBuf[8];
//   dtostrf(temperature, 4, 2, tBuf);
//   dtostrf(humidity, 4, 2, hBuf);

//   mqtt.publish(TOPIC_TEMP, tBuf);
//   mqtt.publish(TOPIC_HUM, hBuf);

//   Serial.print("Published -> Temp: ");
//   Serial.print(tBuf);
//   Serial.print(" C | Hum: ");
//   Serial.print(hBuf);
//   Serial.println(" %");

//   delay(5000); // publish every 5 seconds
// }

/****************************************************
 * ESP32 + DHT22 + MQTT (PUBLISHER ONLY)
 * Topic:
 *   home/lab1/temp
 *   home/lab1/hum
 ****************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ---------- WiFi ----------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ---------- MQTT ----------
const char *mqtt_server = "test.mosquitto.org"; // Mosquitto / Cloud broker
const int mqtt_port = 1883;

// Topics
const char *TOPIC_TEMP = "home/lab1/temp";
const char *TOPIC_HUM = "home/lab1/hum";

// ---------- DHT ----------
#define DHTPIN 23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---------- MQTT Client ----------
WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---------- Functions ----------
void connectWiFi()
{
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT()
{
  while (!mqtt.connected())
  {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect("ESP32_Publisher-1"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  dht.begin();
  connectWiFi();

  mqtt.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void loop()
{
  if (!mqtt.connected())
  {
    connectMQTT();
  }
  mqtt.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  char tBuf[8], hBuf[8];
  dtostrf(temperature, 4, 2, tBuf);
  dtostrf(humidity, 4, 2, hBuf);

  mqtt.publish(TOPIC_TEMP, tBuf);
  mqtt.publish(TOPIC_HUM, hBuf);

  Serial.print("Published -> Temp: ");
  Serial.print(tBuf);
  Serial.print(" C | Hum: ");
  Serial.print(hBuf);
  Serial.println(" %");

  delay(5000); // publish every 5 seconds
}
