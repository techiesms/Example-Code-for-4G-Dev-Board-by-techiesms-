
/**************************************************************

  This is a MQTT Client example code and here we have used
  Adafruit MQTT Platfrom to show the demo by Sending &
  Receving data to & from different MQTT topics using
  4G Development board by techiesms

  Libraries Used:
  1. TinyGSM - https://github.com/vshymanskyy/TinyGSM (Tested with version 0.11.7)
  2. PubSubClient - https://github.com/knolleary/pubsubclient (Tested with version 2.8.0)

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


// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char* broker = "io.adafruit.com";
const char* GsmClientName = "ABC123";
const char* mqtt_user = "";
const char* mqtt_pass = "";

const char* pubTopic = "Sachin_SMS/feeds/ledstatus";
const char* subTopic = "Sachin_SMS/feeds/ledpin";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);


#define LED_PIN 9

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();
  payload[len] = 0;
  String recv_payload = String((char*)payload);

  // Only proceed if incoming message's topic matches
  if (String(topic) == subTopic) {
    if (recv_payload == "1") {
      digitalWrite(LED_PIN,HIGH);
      mqtt.publish(pubTopic, "1");
      Serial.println("Data Published - 1");
    } else {
      digitalWrite(LED_PIN, LOW);
      mqtt.publish(pubTopic, "0");
      Serial.println("Data Published - 0");
    }
  }
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  //boolean status = mqtt.connect("GsmClientTest");

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect(GsmClientName, mqtt_user, mqtt_pass);

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.subscribe(subTopic);
  return mqtt.connected();
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

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {

  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        SerialMon.println("GPRS reconnected");
      }
    }
  }

  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  mqtt.loop();
}
