/**************************************************************

  This is the code for auto call receiving and recording using our
  4G Development board by techiesms


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


  Code Tested on 29/06/2024 by team techiesms

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

#define TINY_GSM_DEBUG SerialMon


// Module baud rate
uint32_t rate = 0;  // Set to 0 for Auto-Detect

String msg;
String fromGSM = "";
#define rec 8
#define DEBUG true

void setup() {
  // Set console baud rate
  SerialMon.begin(9600);
  pinMode(PWR_KEY, OUTPUT);
  pinMode(rec, INPUT_PULLUP);

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
  // Just Checking
  msg = "";
  msg = sendData("AT", 1000, DEBUG);
  while (msg.indexOf("OK") == -1) {
    msg = sendData("AT", 1000, DEBUG);
    Serial.println("Trying");
  }
}

void loop() 
{
  // Listen from GSM Module
  if (SerialAT.available()) {
    fromGSM += (char)SerialAT.read();
    

    // Check if the received message contains "RING"
    if (fromGSM.indexOf("RING") != -1) {
      SerialMon.println("---------ITS RINGING-------");
      sendATCommand("ATA");
      delay(1000);
      sendATCommand("AT+CREC=3,\"c:/recording1.amr\"");
      SerialMon.println("start recording");

      // Clear the buffer after processing
      fromGSM = "";
    }
  }

  if (SerialMon.available()) {
    int inByte = SerialMon.read();
    SerialAT.write(inByte);
  }

  // For recording file play
  int stat = digitalRead(rec);
  if (stat == LOW) {
    sendATCommand("AT+CCMXPLAY=\"c:/recording1.amr\",0,0");
    SerialMon.println("audio file play");
  }
}

String sendData(String command, const int timeout, boolean debug) {
  String temp = "";
  SerialAT.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (SerialAT.available()) {
      char c = SerialAT.read();
      temp += c;
    }
  }
  if (debug) {
    Serial.print(temp);
  }
  return temp;
}

void sendATCommand(const String &command) {
  SerialAT.println(command);
  delay(1000);
  while (SerialAT.available()) {
    SerialMon.write(SerialAT.read());
  }
}
