# Dashboard with esp32

Finally finished my smart workspace display project!! It uses an ESP32 to pull data from a Python server running on my Mac. It updates in real-time to show my top 5 apps (so the esp32 doesn't explode lol) active apps and whatever song or video is now playing. Took a bit to fix the memory drops but we got there.
Warning! everything related to the esp32 code was coded by me, slight debugging from gemini, but the python server and typermonkey scipt were mostly written by gemnini as I don't know python

Now if you want to recreate this abomination, you will need a few items. Your laptop doesnt need to be too strong (I ran this on a macbook air from 2017).
Anyway back to the items, you will need;
	ESP32 Development Board (ESP32 dev board is what I used)
	0.96" SSD1306 OLED Display
	Breadboard & Jumper Wires
	DHT11 Temperature & Humidity Sensor
	Push Buttons
	Breadboard
	Jumper Wires
	Micro USB cable
	Wi-Fi network
Well basically everything you get from a starter esp32 kit, now lets go through the code part.
	Step 1: Copy paste the file that ends with ino into your arduino ide. Note before uploading, You need to open the Library Manager in Arduino IDE and install Adafruit SSD1306, Adafruit GFX Library, ArduinoJson, and DHT sensor library by Adafruit
	Step 2: Connect your esp32 and upload the code into it, you may have to download the driver required for the esp32 and select the board in the tools menu
	step 3: now that the esp32 is ready, we need to wire the parts together. Make sure you wire them properly and don't misplace the wires (burnt my esp32 by accident like that)
	Step 3.5: Before running the python file, open your computer's terminal and install the required parts by running pip3 install flask flask-cors
	step 4: start the .py file on your computer, this opens a server under you computers ip address and sends the time you spent on your top 5 apps and what you are watching/listening too. Be aware that for it to work it should run all the time or atleast when your computer is on to be accurate.
	step 5: download tampermonkey extention and copy past the code named tampermonkey into a new script, allow to run on all site and it should work more or less. (this is for the current playing tab)
	final step: enjoy your new dash board, I will be putting pics of my wiring and also send the wiring.
Here is the pinout:
	OLED Screen: GND -> Breadboard Blue Rail (-), VCC -> Breadboard Red Rail (+), SCL -> GPIO 22, SDA -> GPIO 21
	DHT11 Sensor: VCC -> Breadboard Red Rail (+), GND -> Breadboard Blue Rail (-), DATA -> GPIO 4
	Button 1 (Next): Side A -> GPIO 13, Side B -> Breadboard Blue Rail (-)
	Button 2 (Prev): Side A -> GPIO 12, Side B -> Breadboard Blue Rail (-)
	Button 3 (Select): Side A -> GPIO 14, Side B -> Breadboard Blue Rail (-)
	Button 4 (Mode): Side A -> GPIO 27, Side B -> Breadboard Blue Rail (-)
	Connect a jumper wire from one of the ESP32 GND pins directly into the Breadboard Blue Rail (-).
	Connect another jumper wire from the ESP32 3V3 pin directly into the Breadboard Red Rail (+). 
	This shares the power and ground across the whole board so everything turns on safely!
