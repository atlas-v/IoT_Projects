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
#include <time.h>

// Global Vars

// See pinout diagram for _v2
#define OLED_CLOCK  15
#define OLED_DATA   4
#define OLED_RESET  16
// OLED Used
U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R0, 15, 4,  16);

// lineHeight
int g_lineHeight = 0;
int runCounter = 0;

// Replace with your network credentials (STATION)
const char* ssid = "JRGuestWireless";
const char* password = "jrwelcomesu";




//-----------------------------------------------------------------------------
// wifi connect func
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.print("\nConnecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println('.');
    delay(100);
    WiFi.begin(ssid,password);
  }
  Serial.println(WiFi.localIP());  
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// NTP Func
const char* ntpServer = "pool.ntp.org";
// Offset to EST
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  // Return the time value
  /*
  %A	returns day of week
  %B	returns month of year
  %d	returns day of month
  %Y	returns year
  %H	returns hour
  %M	returns minutes
  %S	returns seconds
  */
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  g_OLED.setCursor(0, g_lineHeight *  5);
  g_OLED.print(&timeinfo,"%B %d %Y %H:%M:%S");
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Init Program
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  g_OLED.begin();
  g_OLED.clear();
  // set font type and size
  // font size 10
  // mf -- Monospace, Full glyph support [256]
  g_OLED.setFont(u8g2_font_profont10_mf);

  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();

  g_OLED.setCursor(0, g_lineHeight);
  g_OLED.print("Initialized...");

  g_OLED.setCursor(0, g_lineHeight *  2);
  g_OLED.sendBuffer();

  // Serial stream baud 9600
  Serial.begin(9600);
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  // Display net stats on board
  g_OLED.clear();
  g_OLED.setCursor(0, g_lineHeight);
  g_OLED.print("IP Address: ");
  g_OLED.setCursor(0, g_lineHeight *  2);
  g_OLED.print(WiFi.localIP());

  // Try to hit the NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Execute Loop
void loop() {
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED)) 
  {
    Serial.println("Reconnecting to WiFi...");
    // call the reconnect func.
    WiFi.disconnect();
    WiFi.reconnect();

    // Write status to buffer
    g_OLED.clear();
    g_OLED.print("Not Connected to Network...");
    g_OLED.sendBuffer();
  }
  // IF connected
  else {
        
    // always set the cursor below IP Address header
    g_OLED.setCursor(0, g_lineHeight *  3);
    g_OLED.print((String)"Wifi Strength: "+WiFi.RSSI()+" dB");
    // Blink the on board LED
    digitalWrite(LED_BUILTIN, 0);
    g_OLED.sendBuffer();
    digitalWrite(LED_BUILTIN, 1);
    // increment
    ++runCounter;
    // Display local time
    printLocalTime();
  }
}
//-----------------------------------------------------------------------------


