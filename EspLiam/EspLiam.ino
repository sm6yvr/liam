#include <EasyTransfer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "MowerState.h"

#define wifi_ssid ""
#define wifi_password ""

#define mqtt_server "192.168.0.20"
#define mqtt_port 1883

#define TOPIC_COMMAND_TO_LIAM "/liam/command"
#define TOPIC_SERVICE_TO_LIAM "/liam/service"

#define TOPIC_STATUS_FROM_LIAM "/liam/status"
#define TOPIC_CONSOLE_FROM_LIAM "/liam/console"
#define TOPIC_ESP_CONSOLE_FROM_LIAM "/esp/console"

WiFiClient espClient;
PubSubClient client;

String a;

const byte numChars = 150;
char receivedChars[numChars];
boolean newData = false;
char data;

void setup(){
  Serial.begin(9600);
  
  setup_wifi();
  client.setClient(espClient);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(TOPIC_COMMAND_TO_LIAM);
      client.subscribe(TOPIC_SERVICE_TO_LIAM);
      client.publish(TOPIC_CONSOLE_FROM_LIAM, String("Starting ESP-uno-test").c_str(), true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  char input[length];

  for (int i = 0; i < length; i++) {
    input[i]=payload[i];
  }
  client.publish(TOPIC_ESP_CONSOLE_FROM_LIAM, "Received data on topic: ", true);
  client.publish(TOPIC_ESP_CONSOLE_FROM_LIAM, topic, true);
  client.publish(TOPIC_ESP_CONSOLE_FROM_LIAM, input, true);

  /* Send all incoming json to liam */
  Serial.println((char*)input);
}


void loop(){

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  receiveJsonFromSerial();
  publishJsonFromLiam();
}

void receiveJsonFromSerial() {
    static boolean recvInProgress = false;
    static byte ndx = 1;
    char startMarker = '{';
    char endMarker = '}';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {

                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '}'; // Append end marker
                ndx++;
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 1;
                newData = true;
            }
        }
        else if (rc == startMarker) {
            recvInProgress = true;
            receivedChars[0] = '{';
        }
    }
}

void publishJsonFromLiam() {
  
    /* Send all json messages from liam to MQTT broker */
    if (newData == true) {
        // Max number chars to receive is 108
        client.publish(TOPIC_CONSOLE_FROM_LIAM, String(receivedChars).c_str(), true);
        newData = false;
    } 
}
