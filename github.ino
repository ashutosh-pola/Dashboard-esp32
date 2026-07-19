#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HijelHID_BLEKeyboard.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Dashboard Page Switcher Button
#define BUTTON_PIN 5
int page = 0;
bool lastButtonState = HIGH;

// BLE Media Control Buttons
#define NEXT_PIN 18
#define BACK_PIN 23   
#define PLAY_PIN 17   

bool lastStateNext = HIGH;
bool lastStateBack = HIGH;
bool lastStatePlay = HIGH;

// WiFi Configs
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server Endpoints
const char* usageURL = "http://192.168.1.183:5000/usage";
const char* mediaURL = "http://192.168.1.183:5000/media";

// Time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 4 * 3600;
const int daylightOffset_sec = 0;

String appData = "";
String mediaTitle = "No Media";
String mediaArtist = "";
int mediaProgress = 0;

struct App { String name; int seconds; };
App apps[10];
int count = 0;

HijelHID_BLEKeyboard keyboard("ESP32 Dashboard", "Ashutosh", 100);

// --- CYBER MATRIX BOOT ANIMATION ---
void cyberBootAnimation() {
  #define CHAR_WIDTH 6
  #define CHAR_HEIGHT 8
  #define COLS (SCREEN_WIDTH / CHAR_WIDTH)
  int yPositions[COLS];
  for (int i = 0; i < COLS; i++) yPositions[i] = random(-64, 0);
  
  for (int frame = 0; frame < 100; frame++) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    for (int x = 0; x < COLS; x++) {
      char randomChar = (char)random(33, 126);
      display.setCursor(x * CHAR_WIDTH, yPositions[x]);
      display.print(randomChar);
      yPositions[x] += CHAR_HEIGHT;
      if (yPositions[x] >= SCREEN_HEIGHT || random(0, 100) > 95) yPositions[x] = 0;
    }
    display.display();
    delay(35);
  }
}

void getAppUsage() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(usageURL);
    if(http.GET() == 200) appData = http.getString();
    http.end();
  }
}

void getMediaStatus() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(mediaURL);
    if(http.GET() == 200) {
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, http.getString())) {
        if (doc["playing"]) {
          mediaTitle = doc["title"].as<String>();
          mediaArtist = doc["artist"].as<String>();
          mediaProgress = doc["progress"].as<int>();
        } else {
          mediaTitle = "Paused / Idle";
          mediaArtist = "";
          mediaProgress = 0;
        }
      }
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(NEXT_PIN, INPUT_PULLUP); 
  pinMode(BACK_PIN, INPUT_PULLUP); 
  pinMode(PLAY_PIN, INPUT_PULLUP); 

  keyboard.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  // Triggers the matrix intro, then moves forward to setup WiFi
  cyberBootAnimation();
  dht.begin();

  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Connecting WiFi...");
  display.display();

  while(WiFi.status() != WL_CONNECTED) delay(500);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  if(keyboard.isConnected()) {
    bool stateNext = digitalRead(NEXT_PIN);
    if(lastStateNext == HIGH && stateNext == LOW) { keyboard.tap(MEDIA_NEXT_TRACK); delay(300); }
    lastStateNext = stateNext;

    bool stateBack = digitalRead(BACK_PIN);
    if(lastStateBack == HIGH && stateBack == LOW) { keyboard.tap(MEDIA_PREV_TRACK); delay(300); }
    lastStateBack = stateBack;

    bool statePlay = digitalRead(PLAY_PIN);
    if(lastStatePlay == HIGH && statePlay == LOW) { keyboard.tap(MEDIA_PLAY_PAUSE); delay(300); }
    lastStatePlay = statePlay;
  }

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return;

  bool buttonState = digitalRead(BUTTON_PIN);
  if(lastButtonState == HIGH && buttonState == LOW) {
    page++;
    if(page > 4) page = 0; 
    delay(200); 
  }
  lastButtonState = buttonState;

  display.clearDisplay();
  display.setTextColor(WHITE);

  // PAGE 0: CLOCK
  if(page == 0) {
    display.setTextSize(2); display.setCursor(0,0);
    display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    display.setTextSize(1); display.setCursor(0,35);
    display.printf("%02d/%02d/%d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  }
  // PAGE 1: SENSORS
  else if(page == 1) {
    float t = dht.readTemperature(); float h = dht.readHumidity();
    display.setTextSize(2); display.setCursor(0,0); display.println("ROOM");
    display.setTextSize(1); display.setCursor(0,30);
    if(isnan(t) || isnan(h)) display.println("Sensor Error");
    else { display.printf("Temp: %.1f C\n", t); display.printf("Hum:  %.1f %%\n", h); }
  }
  // PAGE 2: WI-FI
  else if(page == 2) {
    display.setTextSize(2); display.setCursor(0,0); display.println("SYSTEM");
    display.setTextSize(1); display.setCursor(0,30);
    display.printf("WiFi: OK\nSignal: %d dBm\n", WiFi.RSSI());
  }
  // PAGE 3: APP MONITOR
  else if(page == 3) {
    getAppUsage();
    display.setTextSize(1); display.setCursor(0,0); display.println("APP TIME");
    if(appData.length() > 0) {
      StaticJsonDocument<512> doc;
      if(!deserializeJson(doc, appData)) {
        count = 0;
        for(JsonPair app : doc.as<JsonObject>()) {
          apps[count].name = app.key().c_str(); apps[count].seconds = app.value();
          count++; if(count >= 10) break;
        }
        for(int i = 0; i < count - 1; i++) {
          for(int j = i + 1; j < count; j++) {
            if(apps[j].seconds > apps[i].seconds) { App temp = apps[i]; apps[i] = apps[j]; apps[j] = temp; }
          }
        }
        int y = 15;
        for(int i = 0; i < count; i++) {
          String name = apps[i].name; int s = apps[i].seconds;
          if(name == "Arduino IDE" || name == "Visual Studio Code") name = "Code";
          else if(name == "Microsoft Edge") name = "Edge";
          else if(name == "Terminal") name = "Term";
          else if(name == "Minecraft" || name == "java") name = "Mc";

          String timeText = (s >= 3600) ? String(s / 3600) + "h " + String((s % 3600) / 60) + "m" : (s >= 60) ? String(s / 60) + "m" : String(s) + "s";
          display.setCursor(0, y); display.print(name); display.setCursor(80, y); display.print(timeText);
          y += 10; if(y > 55) break;
        }
      }
    } else { display.setCursor(0, 20); display.println("No Data..."); }
  }
  // PAGE 4: EDGE YOUTUBE MEDIA
  else if(page == 4) {
    getMediaStatus();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("NOW PLAYING");
    
    display.setCursor(0, 16);
    if(mediaTitle.length() > 20) display.println(mediaTitle.substring(0, 18) + "..");
    else display.println(mediaTitle);
    
    display.setCursor(0, 28);
    display.print(mediaArtist);
    
    // Progress Bar Elements
    display.drawRect(0, 46, 128, 7, WHITE);
    int barWidth = (mediaProgress * 126) / 100;
    display.fillRect(1, 47, barWidth, 5, WHITE);
    
    display.setCursor(52, 56);
    display.print(String(mediaProgress) + "%");
  }

  display.display();
  delay(50);
}