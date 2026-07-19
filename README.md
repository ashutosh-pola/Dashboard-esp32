# Dashboard with ESP32

Finally finished my smart workspace display project!! It uses an ESP32 to pull data from a Python server running on my Mac. It updates in real-time to show my top 5 apps (so the esp32 doesn't explode lol) active apps and whatever song or video is now playing. Took a bit to fix the memory drops but we got there. 

> **Warning!** Everything related to the esp32 code was coded by me, slight debugging from Gemini, but the python server and Tampermonkey script were mostly written by Gemini as I don't know Python.

---

## Hardware Requirements
Now if you want to recreate this abomination, you will need a few items. Your laptop doesn't need to be too strong (I ran this on a MacBook Air from 2017). 

Basically, you just need everything you get from a standard ESP32 starter kit:
* **ESP32 Development Board** (ESP32 dev board is what I used)
* **0.96" SSD1306 OLED Display**
* **DHT11 Temperature & Humidity Sensor**
* **4x Push Buttons**
* **Breadboard**
* **Jumper Wires**
* **Micro USB cable**
* **Wi-Fi network**

---

## Setup Steps

### Step 1: Arduino IDE Setup
Copy and paste the file that ends with `.ino` into your Arduino IDE. 

> **Note before uploading:** You need to open the **Library Manager** in Arduino IDE and install these libraries:
> * `Adafruit SSD1306`
> * `Adafruit GFX Library`
> * `ArduinoJson`
> * `DHT sensor library by Adafruit`

### Step 2: Upload Code
Connect your ESP32 and upload the code to it. You may have to download the driver required for the ESP32 (like CP210x) and select your specific board in the Tools menu.

### Step 3: Hardware Wiring
Now that the ESP32 is ready, we need to wire the parts together. Make sure you wire them properly and don't misplace the wires (burnt my ESP32 by accident like that!). 

**Follow this exact pinout:**
* **OLED Screen:** `GND` -> Breadboard Blue Rail (-), `VCC` -> Breadboard Red Rail (+), `SCL` -> GPIO 22, `SDA` -> GPIO 21
* **DHT11 Sensor:** `VCC` -> Breadboard Red Rail (+), `GND` -> Breadboard Blue Rail (-), `DATA` -> GPIO 4
* **Button 1 (Next):** Side A -> GPIO 13, Side B -> Breadboard Blue Rail (-)
* **Button 2 (Prev):** Side A -> GPIO 12, Side B -> Breadboard Blue Rail (-)
* **Button 3 (Select):** Side A -> GPIO 14, Side B -> Breadboard Blue Rail (-)
* **Button 4 (Mode):** Side A -> GPIO 27, Side B -> Breadboard Blue Rail (-)

** CRITICAL POWER STEPS:**
1. Connect a jumper wire from one of the ESP32 `GND` pins directly into the **Breadboard Blue Rail (-)**.
2. Connect another jumper wire from the ESP32 `3V3` pin directly into the **Breadboard Red Rail (+)**. 
*This shares the power and ground across the whole board so everything turns on safely!*

### Step 4: Python Server Setup
Before running the Python file, open your computer's terminal and install the required modules by running:
```bash
pip3 install flask flask-cors
