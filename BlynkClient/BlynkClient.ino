
/**************************************************************

 This is the Blynk Client example code in which 
 we are sending a counter data to Virtual Pin V0 &
 receiving data from virtual pin V1 and Controlling
 LED based on the data recevied using the 4G
 Development board by techiesms.

  Libraries Used:
  1. TinyGSM - https://github.com/vshymanskyy/TinyGSM (Tested with version 0.11.7)
  2. Blynk - https://github.com/blynkkk/blynk-library (Tested with version 1.2.0)

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
  MEGA        |  GND |     2    |  TX1 |  RX1
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  Code Tested on 21/04/2024 by team techiesms

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


#define BLYNK_PRINT Serial  // Comment this out to disable prints and save space


#define TINY_GSM_MODEM_SIM7600

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial port for module based upon the board selected

//For ESP8266
#ifdef ESP8266
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(D5, D6);  // RX, TX
#define PWR_KEY D7                // Power Pin of our Module

// For ESP32
#elif defined(ESP32)
#define SerialAT Serial2
#define PWR_KEY 5  // Power Pin of our Module

// For Arduino MEGA
#elif __AVR_ATmega2560__
#define SerialAT Serial1
#define PWR_KEY 2  // Power Pin of our Module

//For Arduino UNO, NANO
#elif __AVR_ATmega328P__
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(12, 11);  // RX, TX
#define PWR_KEY 13  // Power Pin of our Module


#else
#error "Board not found"
#endif

#define LED_PIN 9

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
const char auth[] = "AUTH_TOKEN";

BlynkTimer timer;

TinyGsm modem(SerialAT);

BLYNK_WRITE(V1) {
  int value = param.asInt();
  Serial.print("Received Data - ");
  Serial.println(value);
  digitalWrite(LED_PIN, value);
}


int i = 0;
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, i++);
  Serial.print(i);
  Serial.println(" Sent");
}



void setup() {
  SerialMon.begin(9600);  // beginning Serial Monitor
  delay(10);

  pinMode(PWR_KEY, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

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

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  Blynk.begin(auth, modem, apn, gprsUser, gprsPass);

  // Setup a function to be called every second
  timer.setInterval(5000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer
}
