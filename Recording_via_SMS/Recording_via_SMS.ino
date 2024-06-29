/**************************************************************

  This is the code for doing Voice Recording via SMS using our 
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

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

int REC_PIN = 8;
String msg;
String phoneNumber = "TARGET MOBILE NUMBER";  // The phone number to send confirmation SMS
#define DEBUG true

void setup() {
  SerialMon.begin(9600);
  pinMode(PWR_KEY, OUTPUT);
  pinMode(REC_PIN, INPUT_PULLUP);

  // Powering Up the 4G Module
  SerialMon.println("Powering Up the 4G Module");
  digitalWrite(PWR_KEY, HIGH);
  delay(50);
  digitalWrite(PWR_KEY, LOW);

  delay(6000);

  SerialMon.println("Initializing modem...");
  SerialAT.begin(9600);

  SerialMon.println("Modem initialized.");
  // Just Checking
  msg = "";
  msg = sendData("AT", 1000, DEBUG);
  while (msg.indexOf("OK") == -1) {
    msg = sendData("AT", 1000, DEBUG);
    Serial.println("Trying ok");
  }

  // For SMS
  msg = "";
  msg = sendData("AT+CMGF=1", 2000, DEBUG);
  while (msg.indexOf("OK") == -1) {
    msg = sendData("AT+CMGF=1", 1000, DEBUG);
    Serial.println("Trying cmgf");
  }

  // msg = "";
  // msg = sendData("AT+CSMP=17,23,0,244", 2000, DEBUG);
  // while ( msg.indexOf("OK") == -1 ) {
  //   msg = sendData("AT+CSMP=17,23,0,244", 1000, DEBUG);
  //   Serial.println("Trying csmp");
  // }

  msg = "";
  msg = sendData("AT+CNMI=1,2,0,0,0", 2000, DEBUG);
  while (msg.indexOf("OK") == -1) {
    msg = sendData("AT+CNMI=1,2,0,0,0", 2000, DEBUG);
    Serial.println("Trying CNMI");
  }
}

void loop() {
  if (SerialAT.available()) {
    String incomingData = SerialAT.readString();
    SerialMon.print("Received: ");
    SerialMon.println(incomingData);

    if (incomingData.indexOf("+CMT:") != -1) {
      handleSMS(incomingData);
    }
  }

  if (SerialMon.available()) {
    SerialAT.write(SerialMon.read());
  }
}

void handleSMS(const String &sms) {
  int idx = sms.indexOf("+CMT:");
  if (idx != -1) {
    int startIdx = sms.indexOf("\"", idx + 6);
    int endIdx = sms.indexOf("\"", startIdx + 1);
    phoneNumber = sms.substring(startIdx + 1, endIdx);

    int bodyStart = sms.indexOf("\r\n", endIdx) + 2;
    String body = sms.substring(bodyStart);

    SerialMon.print("Phone number: ");
    SerialMon.println(phoneNumber);
    SerialMon.print("Message body: ");
    SerialMon.println(body);

    if (body.indexOf("START RECORDING") != -1) {
      SerialMon.println("Starting recording...");
      if (sendATCommand("AT+CREC=1,\"c:/recording4.amr\"").indexOf("OK") != -1) {
        // sendSMS(phoneNumber, "Recording started");
        bool res = modem.sendSMS(phoneNumber, "RECORDING STARTED");
      }
    } else if (body.indexOf("STOP RECORDING") != -1) {
      SerialMon.println("Stopping recording...");
      if (sendATCommand("AT+CREC=0").indexOf("OK") != -1) {
        // sendSMS(phoneNumber, "Recording stopped");
        bool res = modem.sendSMS(phoneNumber, "RECORDING STOPPED");
      }
    } else if (body.indexOf("PLAY RECORDING") != -1) {
      SerialMon.println("Playing recording...");
      if (sendATCommand("AT+CCMXPLAY=\"c:/recording4.amr\",0,0").indexOf("OK") != -1) {
        //sendSMS(phoneNumber, "Playback started");
        bool res = modem.sendSMS(phoneNumber, "PLAYING AUDIO");
      }
    } else if (body.indexOf("STOP PLAYBACK") != -1) {
      SerialMon.println("Stopping playback...");
      if (sendATCommand("AT+CCMXSTOP").indexOf("OK") != -1) {
        // sendSMS(phoneNumber, "Playback stopped");
        bool res = modem.sendSMS(phoneNumber, "AUDIO STOPPED");
      }
    }
  }
}

String sendATCommand(const String &command) {
  SerialAT.println(command);
  delay(1000);
  String response = "";
  while (SerialAT.available()) {
    response += (char)SerialAT.read();
  }
  SerialMon.print(response);
  return response;
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
