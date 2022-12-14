//---------------------------------------------------------------------------//
/*
! Program Name: main
! Author(s):    RMB
! Board:        Heltec-Wifi-32_v2
! Description:  Basic WiFi connection program for the Heltec-Wifi-32_v2
! Init Release: 12/14/2022
! ---------------------------------------------------------------------------
! Revision History
! Rev:    Author:   Date:         Comment:
! 0.0.2   RMB       12/15/2022    Added heap memory monitor and line array
!
!
! To Do:
! - Work on main loop to avoid slow function calls:
!     o NTP
!     o Buffer writes
! - Build internal time reference to do time based calls
! - Set global margins
! - Add network memory, loop through known ssids if no connection
*/
//---------------------------------------------------------------------------//

//  Include Libs
#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <time.h>
#include <string.h>
#include <asyncTCP.h>
#include <ESPAsyncWebServer.h>

// Global Vars
  // Replace with your network credentials (STATION)
  const char *ssid = "JRGuestWireless";
  const char *password = "jrwelcomesu";



  // FIRMWARE REV
  const char *_FIRMWARE = "0.0.2";
  // RELEASE DATE
  const char *_RELEASE_DATE = "12/15/2022";

  // See pinout diagram for _v2
  #define OLED_CLOCK 15
  #define OLED_DATA 4
  #define OLED_RESET 16

  // OLED Used
  /*
  Params:
  Rotation -  U8G2_R0 - none 
              U8G2_R1 - 90
              U8G2_R2 - 180 
              U8G2_R3 - 270
              U8G2_MIRROR - mirrored
              U8G2_MIRROR_VERTICAL	- vertical mirror
  Clock Pin - 15
  Data Pin - 4
  Reset Pin - 16
  */
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R0, 15, 4, 16);

  // init
  int runCounter = 0;
  int g_lineHeight = 0;
  // init max lines and lines
  int maxLines = 0;
  // max expected lines -- 30 for now
  int line[30];
  // heap and strength
  int previousRSSI = 0; 
  int previousHEAP = 0;

  // ERRORS
  int _ACTIVE_ERROR[10];
  
  // Asynchronous HTTP server
  // direct server to port 80
  AsyncWebServer server(80);
  // Request handler
  String request;
  
// Global Vars End


//-----------------------------------------------------------------------------
// Error setup 
void errorInit() 
{
  // Set number of blinks
  _ACTIVE_ERROR[1] = 3; // Initial WiFi connection failed. Check Credentials,
  _ACTIVE_ERROR[2] = 4; // WiFi failed after successful connect, bring within range
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// cursor Init -- abstraction
void cursorInit()
{
  // establish line height
  g_lineHeight = g_OLED.getMaxCharHeight();
  // Establish lines
  Serial.print((String) "\nOLED height:\t" + g_OLED.getHeight());
  Serial.print((String) "\nLine height:\t" + g_lineHeight);

  // max lines
  maxLines = g_OLED.getHeight() / g_lineHeight;
  // line array dictated by maxlines
  for (int i = 0; i <= maxLines; ++i)
  {
    line[i] = i * g_lineHeight;
    Serial.print((String) "\nLine "+i+" Begin: " + line[i]);
  };
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// wifi connect func
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.print("\nConnecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
    g_OLED.setCursor(0, line[1]);
    g_OLED.print("Attempting...");
    g_OLED.setCursor(0, line[2]);
    g_OLED.print("Check Credentials");
    g_OLED.sendBuffer();
    // Error Signal -- initial connect failure
    for (int i = 0; i<_ACTIVE_ERROR[1];++i) 
    {
      digitalWrite(LED_BUILTIN, 1);
      delay(250);
      digitalWrite(LED_BUILTIN,0);
      delay(250);
    }
    // Return network attempted
      Serial.print((String)"\nNetwork Name: "+ssid);
  }
  Serial.println(WiFi.localIP());
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// NTP Func
const char *ntpServer = "pool.ntp.org";
// Offset to EST
const long gmtOffset_sec = -18000;
const int daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
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
  g_OLED.setCursor(0, line[2]);
  g_OLED.print(&timeinfo, "%B %d %Y %H:%M:%S");
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Server handle functions -- contain within this block
void host()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", "Hello world!!");
    Serial.print("\nHandled accepted request");
  });
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Init Program
void setup()
{
  // set onboard LED to writeable
  pinMode(LED_BUILTIN, OUTPUT);
  // call OLED object -- init
  g_OLED.begin();
  // clear OLED
  g_OLED.clear();
  // Serial stream baud 9600
  Serial.begin(9600);
  // set font type and size
  // font size 10
  // mf -- Monospace, Full glyph support [256]
  g_OLED.setFont(u8g2_font_profont10_mf);
  // error setup func call
  errorInit();
  // setup cursor manipulation
  cursorInit();
  
  // User message
  g_OLED.clear();
  for (int i = 0; i < maxLines;++i) 
  {
    g_OLED.setCursor(15, line[i+1]);
    g_OLED.print("Atlas Technologies");
  }
  g_OLED.sendBuffer();
  delay(500);
  
  // boot display
  g_OLED.clear();
  g_OLED.setCursor(0, line[1]);
  g_OLED.print("BOOT");
  g_OLED.sendBuffer();
  for (int i = 0; i < 3; ++i)
  {
    delay(50);
    g_OLED.print(".");
    g_OLED.sendBuffer();
  };
  g_OLED.setCursor(0, line[3]);
  g_OLED.print((String) "Firmware: " + _FIRMWARE);
  g_OLED.setCursor(0, line[4]);
  g_OLED.print((String)"Release: "+_RELEASE_DATE);
  g_OLED.sendBuffer();
  // boot display linger
  delay(1000);
  g_OLED.clearBuffer();

  // Wifi connection func
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  // Display net stats on board
  g_OLED.clear();
  g_OLED.setCursor(0, line[1]);
  g_OLED.print("IP Addr: ");
  g_OLED.print(WiFi.localIP());
  // Try to hit the NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Hosted webserver
  host();
  server.begin();
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Execute Loop
void loop()
{
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
  else
  {
    // Establish CURRENT data references
    int currentRSSI = WiFi.RSSI();
    int currentHEAP = esp_get_free_heap_size()/1000;
    


    // EVENT DRIVEN STATS -- 
    // Only write when we see a change
    // always set the cursor below IP Address header
    if (currentRSSI != previousRSSI) 
    {
      g_OLED.setCursor(0, line[4]);
      g_OLED.print((String) "Wifi Strength: " + WiFi.RSSI() + " dB");
      // Blink the on board LED
      digitalWrite(LED_BUILTIN, 0);
      g_OLED.sendBuffer();
    };
    if (currentHEAP != previousHEAP) 
    {
      // show free mem
      g_OLED.setCursor(0, line[5]);
      g_OLED.print((String) "Free Mem: " + (esp_get_free_heap_size() / 1000) + " kB");
      // Blink the on board LED
      digitalWrite(LED_BUILTIN, 0);
      g_OLED.sendBuffer();
    };
    

    // always OFF unless event
    digitalWrite(LED_BUILTIN, 1);
    
    // Display local time
    printLocalTime();



    // running bar
    if (runCounter > 10000) 
    {
      runCounter = 0;
    };
    // animation test
    g_OLED.setCursor(10, line[6]);
    g_OLED.print(runCounter);




    // Establish PREVIOUS data references
    previousRSSI = currentRSSI; 
    previousHEAP = currentHEAP;
    // increment
    ++runCounter;
  }
}
//-----------------------------------------------------------------------------
