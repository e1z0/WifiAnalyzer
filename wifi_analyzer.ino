/* ESP8266 Wifi Analyzer modified to run on 480x320 TFT RaspberryPI screen connected to esp
 * Uses more modern library to drive lcd screen: https://github.com/Bodmer/TFT_eSPI/
 * Please make sure to read the README.md for more information
 * (c) 2022 e1z0
 * Original source: https://github.com/moononournation/ESP8266WiFiAnalyzer
 */
#include <SPI.h>
#include <TFT_eSPI.h> 
#include "ESP8266WiFi.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <stdio.h>
#include <string.h>
TFT_eSPI tft = TFT_eSPI();  


// Screen configuration
#define WIDTH 480
#define HEIGHT 320
// other configuration
#define UPDATE_INTERVAL 2000
#define MAX_SSID_LEN    7
#define BANNER_HEIGHT 16
// color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198

#define GRAPH_BASELINE (HEIGHT - 18)
#define GRAPH_HEIGHT (HEIGHT - 52)
#define CHANNEL_WIDTH (WIDTH / 16)

// RSSI RANGE
#define RSSI_CEILING -40
#define RSSI_FLOOR -100
#define NEAR_CHANNEL_RSSI_ALLOW -70
#define TFT_WHITE   ILI9341_WHITE   /* 255, 255, 255 */
#define TFT_BLACK   ILI9341_BLACK   /*   0,   0,   0 */
#define TFT_RED     ILI9341_RED     /* 255,   0,   0 */
#define TFT_ORANGE  ILI9341_ORANGE  /* 255, 165,   0 */
#define TFT_YELLOW  ILI9341_YELLOW  /* 255, 255,   0 */
#define TFT_GREEN   ILI9341_GREEN   /*   0, 255,   0 */
#define TFT_CYAN    ILI9341_CYAN    /*   0, 255, 255 */
#define TFT_BLUE    ILI9341_BLUE    /*   0,   0, 255 */
#define TFT_MAGENTA ILI9341_MAGENTA /* 255,   0, 255 */


// Channel color mapping from channel 1 to 14
uint16_t channel_color[] = {
  TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_MAGENTA,
  TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_MAGENTA,
  TFT_RED, TFT_ORANGE
};

uint8_t scan_count = 0;

void setup() {
  tft.begin();

  tft.setRotation(3);

  // init banner

  tft.setTextSize(2);
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setCursor(0, 0);
  tft.print(" ESP ");
  tft.setTextColor(TFT_WHITE, TFT_ORANGE);
  tft.print(" 8266 ");
  tft.setTextColor(TFT_WHITE, TFT_GREEN);
  tft.print(" WiFi ");
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.print(" Analyzer");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // rest for WiFi routine?
  delay(100);
}

void loop() {
  uint8_t ap_count[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int32_t max_rssi[] = {-100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();

  // clear old graph
  tft.fillRect(0, BANNER_HEIGHT, WIDTH, HEIGHT-16, TFT_BLACK);
  tft.setTextSize(1);

  if (n == 0) {
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(0, BANNER_HEIGHT);
    tft.println("no networks found");
  } else {
    // plot found WiFi info
    for (int i = 0; i < n; i++) {
      int32_t channel = WiFi.channel(i);
      int32_t rssi = WiFi.RSSI(i);
      uint16_t color = channel_color[channel - 1];
      int height = constrain(map(rssi, RSSI_FLOOR, RSSI_CEILING, 1, GRAPH_HEIGHT), 1, GRAPH_HEIGHT);

      // channel stat
      ap_count[channel - 1]++;
      if (rssi > max_rssi[channel - 1]) {
        max_rssi[channel - 1] = rssi;
      }

      tft.drawLine(channel * CHANNEL_WIDTH, GRAPH_BASELINE - height, (channel - 1) * CHANNEL_WIDTH, GRAPH_BASELINE + 1, color);
      tft.drawLine(channel * CHANNEL_WIDTH, GRAPH_BASELINE - height, (channel + 1) * CHANNEL_WIDTH, GRAPH_BASELINE + 1, color);

      // Print SSID, signal strengh and if not encrypted
      tft.setTextColor(color);
      tft.setCursor((channel - 1) * CHANNEL_WIDTH, GRAPH_BASELINE - 10 - height);
      tft.print(WiFi.SSID(i));
      tft.print('(');
      tft.print(rssi);
      tft.print(')');
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        tft.print('*');
      }

      // rest for WiFi routine?
      delay(10);
    }
  }

  // print WiFi stat
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, BANNER_HEIGHT);
  tft.print(n);
  tft.print(" networks found, suggested channels: ");
  bool listed_first_channel = false;
  for (int i = 1; i <= 11; i++) { // channels 12-14 may not available
    if ((i == 1) || (max_rssi[i - 2] < NEAR_CHANNEL_RSSI_ALLOW)) { // check previous channel signal strengh
      if ((i == sizeof(channel_color)) || (max_rssi[i] < NEAR_CHANNEL_RSSI_ALLOW)) { // check next channel signal strengh
        if (ap_count[i - 1] == 0) { // check no AP exists in same channel
          if (!listed_first_channel) {
            listed_first_channel = true;
          } else {
            tft.print(", ");
          }
          tft.print(i);
        }
      }
    }
  }

  // draw graph base axle
  tft.drawFastHLine(0, GRAPH_BASELINE, WIDTH, TFT_WHITE);
  for (int i = 1; i <= 14; i++) {
    tft.setTextColor(channel_color[i - 1]);
    tft.setCursor((i * CHANNEL_WIDTH) - ((i < 10)?3:6), GRAPH_BASELINE + 2);
    tft.print(i);
    if (ap_count[i - 1] > 0) {
      tft.setCursor((i * CHANNEL_WIDTH) - ((ap_count[i - 1] < 10)?9:12), GRAPH_BASELINE + 11);
      tft.print('(');
      tft.print(ap_count[i - 1]);
      tft.print(')');
    }
  }

  // Wait a bit before scanning again
  delay(5000);
}
