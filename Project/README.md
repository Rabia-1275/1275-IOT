## Smart Medicine Box (ESP32 + IoT)

An IoT-based Smart Medicine Box designed to help patients take medicines on time and allow caregivers to monitor compliance remotely.The system uses ESP32, Blynk IoT, and MQTT (HiveMQ + Node-RED) to provide automated dispensing, real-time alerts, and missed-dose notifications.

## Features

## Automated Medicine Scheduling

Dispenses medicines from 3 compartments at predefined times

## Multi-Modal Alerts

Buzzer, LEDs, OLED display, and mobile notifications

## Compliance Verification

IR sensors detect if medicine is taken

## Remote Monitoring

Real-time data via Blynk mobile app and MQTT dashboards

## Missed Dose Detection

Alerts caregiver if medicine is not taken within 60 seconds

## Environment Monitoring

Temperature & humidity tracking using DHT11

## Hardware Components

Component Quantity
ESP32 1
Servo Motors 3
IR Sensors 3
DHT11 Sensor 1
OLED Display (SSD1306) 1
LEDs 3
Buzzer 1

## Software & Libraries

- Arduino IDE
- Blynk IoT Platform
- MQTT (HiveMQ Broker)
- Node-RED Dashboard

WiFi.h
BlynkSimpleEsp32.h
ESP32Servo.h
DHT.h
Adafruit_GFX.h
Adafruit_SSD1306.h
PubSubClient.h

## Future Enhancements

- Cloud-based scheduling via Blynk
- Voice assistant integration (Alexa / Google Assistant)
- Pill counting using load cells
- Database storage for medication history
- RTC or NTP time synchronization

## Team Members

- Laiba Taseen (23-NTU-CS-1258)
- Rabia Naseer (23-NTU-CS-1275)
- Malaika (23-NTU-CS-1291)
  Developed as part of Embedded IoT Systems (CSE-3079)
  National Textile University, Faisalabad.
