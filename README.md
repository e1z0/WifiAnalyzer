# Wifi Analyzer


<img src="https://user-images.githubusercontent.com/7213361/203673013-d671208d-3836-4859-a601-ffce42353c23.jpeg" width="600">


WiFi Analyzer can help you to identify Wi-Fi problems, find the best channel or the best place for your router/access-point. Now it works in your esp8266 device with connected large 3.5inch lcd screen!

# Tested with these esp8266 boards

* LOLIN(WEMOS) D1
* Amica NodeMCU

# Wiring

<img src="https://user-images.githubusercontent.com/7213361/203673580-aa3ecb18-aaea-4423-a9be-61f168431743.png" width="400">

* Display SDO/MISO to NodeMCU pin D6
* Display LED to NodeMCU pin VIN (or 5V)
* Display SCK to NodeMCU pin D5
* Display SDI/MOSI to NodeMCU pin D7
* Display DC (or AO)to NodeMCU pin D3
* Display RESET to NodeMCU pin D4 (or RST, see below)
* Display CS to NodeMCU pin D8 (or GND, see below)
* Display GND to NodeMCU pin GND (0V)
* Display VCC to NodeMCU 5V or 3.3V
* Display T_CS to NodeMCU pin D2

The TFT RESET pin can be connected to the NodeMCU RST pin or 3.3V to free up a control pin. The NodeMCU D0 pin can be used for RST.

# Firmware

When compiling with Arduino IDE, make sure you already have library: https://github.com/Bodmer/TFT_eSPI
Also copy file **ser_Setup.h** to **~/Documents/Arduino/libraries/TFT_eSPI** (directory is different in each operating system)

# Background

It's based on https://github.com/moononournation/ESP8266WiFiAnalyzer with modifications to support TFT 480x320 RaspberryPI LCD Screen


# Another similar projects and sources

* https://github.com/Bodmer/TFT_eSPI Library used in this project
* https://github.com/headHUB/ESP8266WiFiAnalyzer ESP32/8266 using ILI9341 320x240 screen (father of wifi analyzer on esp devices)
* https://github.com/theuwis/ESP8266-WiFi-Analyzer Original source of this project
