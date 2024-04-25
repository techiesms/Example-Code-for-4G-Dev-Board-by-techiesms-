/**************************************************************

  This sketch connects to a website and downloads a page
  It can be used to perform HTTP/RESTful API calls using
  4G Development board by techiesms

  Libraries Used:
  1. TinyGSM - https://github.com/vshymanskyy/TinyGSM (Tested with version 0.11.7)

  Boards Package Versions
  ESP32 (2.0.6)
  ESP8266 (3.1.2)


  +++++++++++++++ Connections +++++++++++++++

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  4G Module   |  GND |  PWRKEY  |  RX  |  TX
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ESP32       |  GND |    D5    |  TX2 |  RX2
  -------------------------------------------
  ESP8266     |  GND |    D7    |  D6  |  D5
  -------------------------------------------
  UNO         |  GND |    13    |  11  |  12
  -------------------------------------------
  MEGA        |  GND |     2    |  TX1 | RX1
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  Code Tested on 15/04/2024 by team techiesms

  You can purchase this 4G Module and lot other Electronic
  Components & Projects from our website
  https://www.techiesms.com

  If you are interested in learning Electronics, IOT & Automation
  related projects? Well, then definitely checkout our YouTube channel
  Right Now!!!
  https://www.youtube.com/techiesms

  If you got helped with our code, considering supporting us on Patreon
  https://www.patreon.com/techiesms



**************************************************************/


#define TINY_GSM_MODEM_SIM7600

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial port for module based upon the board selected

//For ESP8266
#ifdef ESP8266
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(D5, D6);  // RX, TX
#define PWR_KEY D7

// For ESP32
#elif defined(ESP32)
#define SerialAT Serial2
#define PWR_KEY 5

// For Arduino MEGA
#elif __AVR_ATmega2560__
#define SerialAT Serial1
#define PWR_KEY 2

//For Arduino UNO, NANO
#elif __AVR_ATmega328P__
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(12, 11);  // RX, TX
                                  // Power Pin of our Module
#define PWR_KEY 13


#else
#error "Board not found"
#endif


// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon


// Uncomment this if you want to use SSL
// #define USE_SSL


// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "";
const char gprsUser[] = "";
const char gprsPass[] = "";


// Server details
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
const int port = 80;


void setup() {
  SerialMon.begin(9600);  // beginning Serial Monitor
  delay(10);

  pinMode(PWR_KEY, OUTPUT);

  // Powering Up the 4G Module
  SerialMon.println("");
  SerialMon.println("techiesms 4G Development Board");
  SerialMon.println("---------Powering Up----------");
  digitalWrite(PWR_KEY, HIGH);
  delay(50);
  digitalWrite(PWR_KEY, LOW);

  SerialMon.println("Wait...");
  SerialAT.begin(9600);  // beginning Serial communication with 4G Module

  // Waiting....
  delay(6000);


  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  //modem.restart(); SerialMon.println("Resetting Modem...");
  modem.init();
  SerialMon.println("Initialising Modem...");

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }

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
}

void loop() {




  SerialMon.print("Connecting to ");
  SerialMon.println(server);
  if (!client.connect(server, port)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  // Make a HTTP GET request:
  SerialMon.println("Performing HTTP GET request...");
  client.print(String("GET ") + resource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.println();

  uint32_t timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data
    while (client.available()) {
      char c = client.read();
      SerialMon.print(c);
      timeout = millis();
    }
  }
  SerialMon.println();

  // Shutdown

  client.stop();
  SerialMon.println(F("Server disconnected"));


  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  // Powering Off 4G Module

  //SerialMon.println("Powering Off 4G Module!");
  //digitalWrite(PWR_KEY, HIGH);
  //delay(2500);
  //digitalWrite(PWR_KEY, LOW);

  // Do nothing forevermore
  while (true) {
    delay(1000);
  }
}
