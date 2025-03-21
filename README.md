# WiFi Phishing Captive Portal

## Overview
This project sets up a fake WiFi access point labeled "Google Free WiFi" using an ESP32 microcontroller. The system hosts a deceptive login page to collect entered credentials and logs them to an SD card. It also detects connected devices and can trigger an alert using an audio file and LED indicators.

## Features
- **Captive Portal:** Forces users to the login page when they connect to the WiFi network.
- **Phishing Page:** Mimics a Google login to capture email and password credentials.
- **Logging System:** Stores entered credentials on an SD card.
- **Device Detection:** Lists connected MAC addresses.
- **Alert System:** Plays an alert sound and changes LED colors when credentials are submitted.
- **File Hosting:** Serves images from an SD card for the login page.

## Hardware Requirements
- **ESP32 development board** (NodeMCU ESP32, ESP32-WROOM, etc.)
- **MicroSD card and SD card reader module**
- **RGB LED for status indication**
- **I2S-compatible speaker module for audio alerts**
- **USB cable for flashing the ESP32**
- **Computer with Arduino IDE and ESP32 board libraries installed**

## Pin Connections

| Component       | ESP32 Pin |
|----------------|-----------|
| SD Card CS     | GPIO 5    |
| I2S BCLK       | GPIO 27   |
| I2S LRC        | GPIO 26   |
| I2S DOUT       | GPIO 25   |
| LED Red        | GPIO 15   |
| LED Green      | GPIO 2    |
| LED Blue       | GPIO 4    |

## Software Requirements
- **Arduino IDE** (latest version recommended)
- **ESP32 Board Manager installed in Arduino IDE**
- **Required libraries:**
  - `WiFi.h`
  - `WebServer.h`
  - `DNSServer.h`
  - `SPI.h`
  - `SD.h`
  - `AudioFileSourceSD.h`
  - `AudioGeneratorMP3.h`
  - `AudioOutputI2S.h`
  - `esp_wifi.h`
  - `driver/ledc.h`

## Installation Steps

### 1. Setup Arduino IDE
- Install the ESP32 board manager.
- Add required libraries via the Library Manager or manually.

### 2. Flashing the ESP32
- Connect the ESP32 board via USB.
- Open the provided Arduino sketch.
- Set the correct board type (`ESP32 Dev Module`).
- Select the appropriate COM port.
- Click "Upload" to flash the code to the ESP32.

### 3. Preparing the SD Card
- Format the microSD card as FAT32.
- Create a file named `log.txt` for credential storage.
- Add an `alert.mp3` file for the alert sound.
- Copy necessary image files (`wifi_icon.png`, `company_logo.png`) for the login page.
- Insert the SD card into the SD card reader module.

### 4. Powering the ESP32
- Once flashed, restart the ESP32.
- It will create a WiFi network named `Google Free WiFi`.

### 5. Captive Portal and Credential Logging
- Any connected user is redirected to the fake login page.
- Entered credentials are saved to `log.txt` on the SD card.
- The device list updates every 5 seconds, displaying MAC addresses of connected clients.
- An audio alert is played and the LED changes color when credentials are submitted.

## Security & Ethical Considerations
This project is intended for **educational and cybersecurity awareness purposes only**. Unauthorized use of this system to steal credentials is **illegal and unethical**. Ensure compliance with local laws before deploying such systems.

## Disclaimer
The author does not condone illegal hacking activities. Use this project responsibly for ethical research, penetration testing, and cybersecurity awareness training.

