// Select your modem:
 #define TINY_GSM_MODEM_SIM808
 
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#define TINY_GSM_RX_BUFFER 650

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
//#define LOGGING  // <- Logging is for the HTTP library

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay - may be needed for a fast processor at a slow baud rate
//#define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

//GPS Location
const int RXPin = 4, TXPin = 3;

// Your GPRS credentials, if any
const char apn[]  = "Econet";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[]  = "developernetwork";
const char wifiPass[] = "Idevelopiot@2020";

// Server details
const char server[] = "https://animal-tracking-api.herokuapp.com";
const int  port = 80;

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif


// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

TinyGPSPlus gps;
TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT,GSM_AUTOBAUD_MIN,GSM_AUTOBAUD_MAX);
  SerialAT.begin(9600);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }
#endif
}

void loop() { 
    // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
          if (gps.location.isValid())
            {
              Serial.print(gps.location.lat(), 6);
              Serial.print(F(","));
              Serial.print(gps.location.lng(), 6);
              #if TINY_GSM_USE_WIFI
              // Wifi connection parameters must be set before waiting for the network
              SerialMon.print(F("Setting SSID/password..."));
              if (!modem.networkConnect(wifiSSID, wifiPass)) {
                SerialMon.println(" fail");
                delay(10000);
                return;
              }
              SerialMon.println(" success");
            #endif
            
            #if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
              // The XBee must run the gprsConnect function BEFORE waiting for network!
              modem.gprsConnect(apn, gprsUser, gprsPass);
            #endif
            
              SerialMon.print("Waiting for network...");
              if (!modem.waitForNetwork()) {
                SerialMon.println(" fail");
                delay(10000);
                return;
              }
              SerialMon.println(" success");
            
              if (modem.isNetworkConnected()) {
                SerialMon.println("Network connected");
              }
            
            #if TINY_GSM_USE_GPRS
              // GPRS connection parameters are usually set after network registration
                SerialMon.print(F("Connecting to "));
                SerialMon.print(apn);
                if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
                  SerialMon.println(" fail");
                  delay(10000);
                  return;
                }
                SerialMon.println(" success");
            
                if (modem.isGprsConnected()) {
                  SerialMon.println("GPRS connected");
                }
            #endif
            
              SerialMon.print(F("Performing HTTP POST request... "));\
              String request = "/track";
              String qm = "?";
              String lat_key = "latitude=";
              String lat_val = (String) gps.location.lat();
              String ape = "&";
              String lng_key = "longitude=";
              String lng_val = (String) gps.location.lng();
              String resource = request + qm + lat_key + lat_val + ape + lng_key + lng_val;
              int err = http.post(resource);
              if (err != 0) {
                SerialMon.println(F("failed to connect"));
                delay(10000);
                return;
              }
            
              int status = http.responseStatusCode();
              SerialMon.print(F("Response status code: "));
              SerialMon.println(status);
              if (!status) {
                delay(10000);
                return;
              }
            
              SerialMon.println(F("Response Headers:"));
              while (http.headerAvailable()) {
                String headerName = http.readHeaderName();
                String headerValue = http.readHeaderValue();
                SerialMon.println("    " + headerName + " : " + headerValue);
              }
            
              int length = http.contentLength();
              if (length >= 0) {
                SerialMon.print(F("Content length is: "));
                SerialMon.println(length);
              }
              if (http.isResponseChunked()) {
                SerialMon.println(F("The response is chunked"));
              }
            
              String body = http.responseBody();
              SerialMon.println(F("Response:"));
              SerialMon.println(body);
            
              SerialMon.print(F("Body length is: "));
              SerialMon.println(body.length());
            
              // Shutdown
            
              http.stop();
              SerialMon.println(F("Server disconnected"));
            
            #if TINY_GSM_USE_WIFI
                modem.networkDisconnect();
                SerialMon.println(F("WiFi disconnected"));
            #endif
            #if TINY_GSM_USE_GPRS
                modem.gprsDisconnect();
                SerialMon.println(F("GPRS disconnected"));
            #endif
            
              // Do nothing forevermore
              while (true) {
                delay(1000);
              }
            
              if (millis() > 5000 && gps.charsProcessed() < 10)
              {
                Serial.println(F("No GPS detected: check wiring."));
                while(true);
              }
            }
            else
            {
              Serial.print(F("INVALID"));
            }

}
