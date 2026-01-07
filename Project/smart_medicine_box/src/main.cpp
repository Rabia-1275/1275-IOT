#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6CbwW83wK"
#define BLYNK_TEMPLATE_NAME "MedicineBox"
#define BLYNK_AUTH_TOKEN "iW81PFdIRoJU4E0y6adMvi0WwObQjnkV"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <PubSubClient.h>

char ssid[] = "T";
char pass[] = "milly020804";

/******************** MQTT ********************/
const char *mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient mqttClient(espClient);

/******************** PINS ********************/
#define DHTPIN 19
#define DHTTYPE DHT11
#define SERVO1_PIN 18
#define SERVO2_PIN 13
#define SERVO3_PIN 33
#define LED1_PIN 25
#define LED2_PIN 26
#define LED3_PIN 27
#define BUZZER_PIN 4
#define IR1_PIN 34
#define IR2_PIN 35
#define IR3_PIN 32

/******************** OBJECTS ********************/
Adafruit_SSD1306 display(128, 64, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);
Servo servo1, servo2, servo3;
BlynkTimer timer;

struct Medicine
{
  int compartment;
  unsigned long timeMs;
  bool done;
};

Medicine medicines[3] = {
    {1, 10000, false}, {2, 20000, false}, {3, 30000, false}};

unsigned long startTime = 0;
int activeCompartment = 0;

/******************** MQTT FUNCTIONS ********************/
void reconnectMQTT()
{
  while (!mqttClient.connected())
  {
    if (mqttClient.connect("MedicineBoxESP32"))
    {
      mqttClient.publish("medbox/status", "ESP32 Connected");
    }
    else
    {
      delay(2000);
    }
  }
}

/******************** FUNCTIONS ********************/
void showOLED(const char *l1, const char *l2)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(l1);
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.println(l2);
  display.display();
}

void turnOffAll()
{
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);

  if (activeCompartment == 1)
  {
    servo1.attach(SERVO1_PIN);
    servo1.write(0);
    delay(1000);
    servo1.detach();
  }
  else if (activeCompartment == 2)
  {
    servo2.attach(SERVO2_PIN);
    servo2.write(0);
    delay(1000);
    servo2.detach();
  }
  else if (activeCompartment == 3)
  {
    servo3.attach(SERVO3_PIN);
    servo3.write(0);
    delay(1000);
    servo3.detach();
  }

  mqttClient.publish("medbox/compartment", "0");

  if (Blynk.connected())
  {
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
    Blynk.virtualWrite(V7, 0);
  }
  activeCompartment = 0;
}

void checkMissedDose()
{
  if (activeCompartment != 0)
  {
    mqttClient.publish("medbox/alert", "Missed Dose");
    if (Blynk.connected())
      Blynk.logEvent("missed_dose", "Reminder: You missed your medication!");
    turnOffAll();
    showOLED("MISSED", "Box Closed");
    timer.setTimeout(3000L, []()
                     { showOLED("Medicine Box", "Ready"); });
  }
}

void updateSensors()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int ir1 = digitalRead(IR1_PIN);
  int ir2 = digitalRead(IR2_PIN);
  int ir3 = digitalRead(IR3_PIN);

  char buf[50];
  sprintf(buf, "%.1f", t);
  mqttClient.publish("medbox/temperature", buf);
  sprintf(buf, "%.1f", h);
  mqttClient.publish("medbox/humidity", buf);
  sprintf(buf, "IR1:%d IR2:%d IR3:%d", ir1, ir2, ir3);
  mqttClient.publish("medbox/ir", buf);

  if (Blynk.connected())
  {
    Blynk.virtualWrite(V8, t);
    Blynk.virtualWrite(V9, h);
    Blynk.virtualWrite(V10, ir1);
    Blynk.virtualWrite(V11, ir2);
    Blynk.virtualWrite(V12, ir3);
  }

  display.fillRect(0, 52, 128, 12, SSD1306_BLACK);
  display.setCursor(0, 54);
  display.setTextSize(1);
  display.printf("T:%.0fC H:%.0f%% IR:%d%d%d", t, h, ir1, ir2, ir3);
  display.display();
}

void dispenseMedicine(int comp)
{
  activeCompartment = comp;
  mqttClient.publish("medbox/dispense", String(comp).c_str());
  mqttClient.publish("medbox/compartment", String(comp).c_str());

  digitalWrite(BUZZER_PIN, HIGH);
  if (comp == 1)
    digitalWrite(LED1_PIN, HIGH);
  if (comp == 2)
    digitalWrite(LED2_PIN, HIGH);
  if (comp == 3)
    digitalWrite(LED3_PIN, HIGH);

  showOLED("ALARM!", "Check Box");
  delay(5000);

  if (Blynk.connected())
    Blynk.logEvent("medicine_time", "Time for your medicine!");

  if (comp == 1)
  {
    servo1.attach(SERVO1_PIN);
    servo1.write(180);
    delay(1000);
    Blynk.virtualWrite(V3, 1);
  }
  if (comp == 2)
  {
    servo2.attach(SERVO2_PIN);
    servo2.write(180);
    delay(1000);
    Blynk.virtualWrite(V4, 1);
  }
  if (comp == 3)
  {
    servo3.attach(SERVO3_PIN);
    servo3.write(180);
    delay(1000);
    Blynk.virtualWrite(V7, 1);
  }

  timer.setTimeout(60000L, checkMissedDose);
}

BLYNK_WRITE(V1)
{
  if (param.asInt() == 1)
  {
    digitalWrite(BUZZER_PIN, LOW);
    showOLED("SILENCED", "Lid stays open");
  }
}

/******************** SETUP ********************/
void setup()
{
  Serial.begin(115200);

  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  mqttClient.setServer(mqtt_server, 1883);

  timer.setInterval(1000L, updateSensors);

  startTime = millis();
  turnOffAll();
  showOLED("System", "Online");
}

/******************** LOOP ********************/
void loop()
{
  if (!mqttClient.connected())
    reconnectMQTT();
  mqttClient.loop();

  if (WiFi.status() == WL_CONNECTED)
    Blynk.run();
  timer.run();

  unsigned long now = millis() - startTime;

  for (int i = 0; i < 3; i++)
  {
    if (!medicines[i].done && now >= medicines[i].timeMs)
    {
      medicines[i].done = true;
      dispenseMedicine(medicines[i].compartment);
    }
  }

  if (activeCompartment != 0)
  {
    int targetPin = (activeCompartment == 1) ? IR1_PIN : (activeCompartment == 2) ? IR2_PIN
                                                                                  : IR3_PIN;
    if (digitalRead(targetPin) == HIGH)
    {
      delay(150);
      if (digitalRead(targetPin) == HIGH)
      {
        mqttClient.publish("medbox/status", "Medicine Taken");
        turnOffAll();
        showOLED("THANK YOU", "Med Taken");
      }
    }
  }
}