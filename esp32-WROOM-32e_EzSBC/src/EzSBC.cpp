//---------------------------------------------------------------------------//
/*
! Program Name: EzSBC
! Author(s):    RMB
! Board:        ESP32-WROOM-32E
! Description:  Wroom script detecting wifi signal strength
! Init Release: 12/14/2022
! ---------------------------------------------------------------------------
! Revision History
! Rev:    Author:   Date:         Comment:
! 0.0.1   RMB       12/18/2022    initial
!
!
! To Do:
! - accelerometer driver
! - atmospheric sensor driver
! - integrate 128x128 OLED
*/
//---------------------------------------------------------------------------//

// Includes
#include <WiFi.h>


// Init Global Variables

// the number of the onboard LED pin
const int redPin    = 16; // GPIO16 -- red
const int greenPin  = 17; // GPIO17 -- green

// rgb pins config
const int rgb_redPin    = 25;
const int rgb_greenPin  = 26;
const int rgb_bluePin   = 27;


// setting PWM properties
const int freq = 5000;
const int ledChannel_0 = 0;
const int ledChannel_1 = 1;
const int resolution = 8;

// Set web server port number to 80
WiFiServer server(80);


// Network Credentials
const char* ssid = "HoloNet";
const char* password = "Coruscant";

void setupLED() {
  // configure LED PWM functionalities
  ledcSetup(ledChannel_0, freq, resolution);
  ledcSetup(ledChannel_1, freq, resolution);


  // attach the channel(s) to the GPIO to be controlled
  ledcAttachPin(redPin, ledChannel_0);
  ledcAttachPin(greenPin, ledChannel_1);
  
};

//-----------------------------------------------------------------------------
void firmware() {
  // Init
  Serial.println("\n\nRunning Firmware: 0.0.1");
};
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
void wifiSetup() {
  // Set the wifi mode to slave
  WiFi.mode(WIFI_STA);
  // Attempt a login on specified network
  WiFi.begin(ssid, password);
  // fist connect delay
  delay(1000);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  // Show the connection
  Serial.println((String)"\nConnected to: " + ssid);
  // Show the boards IP Address
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  // Show the boards MAC Address
  Serial.println((String)"\nMAC: " + WiFi.macAddress());
  Serial.println(".......................");
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Initialize Board
void setup(){
  // Open the serial port at buadrate 9600
  Serial.begin(9600);
  // Run the firmware init prog
  firmware();
  // Run the boot prog
  wifiSetup();
  

  // Run the LED channel setup
  setupLED();

  // start server
  server.begin();
  // pinmodes
  pinMode(rgb_redPin,OUTPUT);
  pinMode(rgb_greenPin,OUTPUT);
  pinMode(rgb_bluePin,OUTPUT);
};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Runtime loop
void loop() {
  // SERVER
  WiFiClient client = server.available();
  // LED display based on signal strength
  // if low strength emit red
  if (WiFi.RSSI() < -55) {
    // Turn on red led for poor signal
    ledcWrite(ledChannel_0, 150); // red LED -- lower pulse higher brightness
    ledcWrite(ledChannel_1, 255); // green LED -- lower pulse higher brightness
    digitalWrite(rgb_redPin, LOW);
    digitalWrite(rgb_bluePin, HIGH);
    digitalWrite(rgb_greenPin, HIGH);
    // Log the strength
    //Serial.println((String)"Signal Strength: "+WiFi.RSSI() + "dB");
  }
  // if medium strength signal emit yellow 
  else if (WiFi.RSSI() < -51) {
    ledcWrite(ledChannel_0, 100); // red LED -- lower pulse higher brightness
    ledcWrite(ledChannel_1, 100); // green LED -- lower pulse higher brightness
    digitalWrite(rgb_redPin, HIGH);
    digitalWrite(rgb_bluePin, LOW);
    digitalWrite(rgb_greenPin, HIGH);
  }
  // if high strength emit green
  else {
    ledcWrite(ledChannel_0, 255); // red LED -- lower pulse higher brightness
    ledcWrite(ledChannel_1, 150); // green LED -- lower pulse higher brightness
    digitalWrite(rgb_redPin, HIGH);
    digitalWrite(rgb_bluePin, HIGH);
    digitalWrite(rgb_greenPin, LOW);
    // Log the strength
    //Serial.println((String)"Signal Strength: "+WiFi.RSSI() + "dB");
  }

  // when a request is processed return active signal strength
  if (client.connected()) {
    Serial.println("New Client");
    client.println(WiFi.RSSI());
    // when session terminates
    client.stop();
    Serial.println("Client Disconnected");
  };

  // exit conditions
  
};
//-----------------------------------------------------------------------------
