/**************************************************************

  This sketch is for sending SMS using
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


  Code Tested on 16/04/2024 by team techiesms

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

// Target Number on which you want to sens SMS
String SMS_TARGET = "+91xxxxxxxxxx";

// Type the message you want to send
String MESSAGE = "Hi, I'm 4G Development board made by techiesms";

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

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);



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
  modem.init();SerialMon.println("Initialising Modem...");

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
}

void loop() {

  // Code to Send SMS
  bool res = modem.sendSMS(SMS_TARGET, MESSAGE);
  DBG("SMS:", res ? "Sent" : "Not Sent");


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
