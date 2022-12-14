//---------------------------------------------------------------------------//
/*
! Program Name: main
! Author(s):    RMB
! Board:        Heltec-Wifi-32_v2
! Description:  Basic WiFi connection program for the Heltec-Wifi-32_v2
!
! Revision:     0.0.1 
! Release Date: 12/14/2022
*/
//---------------------------------------------------------------------------//


//  Include Libs
#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>

// Global Vars

// See pinout diagram for _v2
#define OLED_CLOCK  15
#define OLED_DATA   4
#define OLED_RESET  16
// OLED Used
U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R0, 15, 4,  16);
// Specify line height
int g_lineHeight = 0;

// Replace with your network credentials (STATION)
const char* ssid = "JRGuestWireless";
const char* password = "jrwelcomesu";

// Init first millisecond reading
unsigned long previousMillis = 0;
// Timeout set == 3s
unsigned long interval = 3000;

// wifi connect func
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println('.');
    delay(1000);
    WiFi.begin(ssid,    password);
  }
  Serial.println(WiFi.localIP());  
}



//-----------------------------------------------------------------------------
// Init Program
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont10_tf);

  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();

  g_OLED.setCursor(0, g_lineHeight);
  g_OLED.print("Initialized...");

  g_OLED.setCursor(0, g_lineHeight *  2);
  g_OLED.sendBuffer();

  Serial.begin(9600);
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  // Display net stats
  g_OLED.clear();
  g_OLED.setCursor(0, g_lineHeight);
  g_OLED.print("IP Address: ");
  g_OLED.setCursor(0, g_lineHeight *  2);
  g_OLED.print(WiFi.localIP());
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Execute Loop
void loop() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) 
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  // IF connected
  else {
        
    // always set the cursor below IP Address header
    g_OLED.setCursor(0, g_lineHeight *  3);
    g_OLED.print((String)"Wifi: "+WiFi.RSSI()+" dB");
    // Blink the on board LED
    digitalWrite(LED_BUILTIN, 1);
    g_OLED.sendBuffer();
    digitalWrite(LED_BUILTIN, 0);
    delay(100); 
  }
}
//-----------------------------------------------------------------------------