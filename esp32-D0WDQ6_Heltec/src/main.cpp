#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>


#define OLED_CLOCK  15
#define OLED_DATA   4
#define OLED_RESET  16
// OLED Used
U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R0, 15, 4,  16);
int g_lineHeight = 0;

#include <WiFi.h>

// Replace with your network credentials (STATION)
const char* ssid = "JRGuestWireless";
const char* password = "jrwelcomesu";

unsigned long previousMillis = 0;
unsigned long interval = 30000;


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



void setup() 
{
pinMode(LED_BUILTIN, OUTPUT);
g_OLED.begin();
g_OLED.clear();
g_OLED.setFont(u8g2_font_profont15_tf);

g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();

g_OLED.setCursor(0, g_lineHeight);
g_OLED.print("Hi Raechel :)");

g_OLED.setCursor(0, g_lineHeight *  2);
g_OLED.printf("Download # ");
g_OLED.sendBuffer();

Serial.begin(9600);
initWiFi();
Serial.print("RSSI: ");
Serial.println(WiFi.RSSI());

}

void loop() 
{

digitalWrite(LED_BUILTIN, 1);
delay(100);
digitalWrite(LED_BUILTIN, 0);
delay(400); 

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
}