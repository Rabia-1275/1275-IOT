## ESP32 Environment Monitor (Webserver & Blynk)

## Overview

This project monitors temperature and humidity using an ESP32 and a DHT22 sensor. It displays data locally on an OLED screen and transmits it via two methods: a Local Webserver and the Blynk IoT Cloud.

## How It Works

1. Local Webserver (webserver.cpp)

- The ESP32 acts as a mini-website host.
- Access: Enter the ESP32's IP address into any browser on the same Wi-Fi.
- Control: Press the physical Button to trigger a sensor update.
- Display: The webpage auto-refreshes every 5 seconds to show the latest Temperature and Humidity.

2. Blynk Cloud (blynk.cpp)

- The ESP32 connects to the internet to send data to the Blynk App.
- Remote View: View live data from anywhere in the world using the Blynk mobile app or web dashboard.
- Virtual Pins: Uses V0 (Temp) and V1 (Hum) for stable data transmission.
- Timer: Uses BlynkTimer to send data every 5 seconds without crashing the connection.

## Author: Rabia Naseer

## Date: December 17, 2025

## Course: Embedded IoT Systems Fall 2025
