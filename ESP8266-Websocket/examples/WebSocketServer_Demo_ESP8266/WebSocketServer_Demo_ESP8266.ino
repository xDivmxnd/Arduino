#include <SPI.h>

#include <ESP8266WiFi.h>

// Enabe debug tracing to Serial port.
#define DEBUGGING

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64

// Define how many callback functions you have. Default is 1.
#define CALLBACK_FUNCTIONS 1

#include <WebSocketServer.h>

const char* ssid = "TP-LINK_Lee";
const char* password = "00180605";
WiFiServer server(80);
WebSocketServer webSocketServer;


// Called when a new message from the WebSocket is received
// Looks for a message in this form:
//
// DPV
//
// Where:
//        D is either 'd' or 'a' - digital or analog
//        P is a pin #
//        V is the value to apply to the pin
//

void handleClientData(String &dataString) {
  bool isDigital = dataString[0] == 'd';
  int pin = dataString[1] - '0';
  int value;

  value = dataString[2] - '0';


  pinMode(pin, OUTPUT);

  if (isDigital) {
    digitalWrite(pin, value);
  } else {
    analogWrite(pin, value);
  }

  Serial.println(dataString);
}

// send the client the analog value of a pin
void sendClientData(int pin) {
  String data = "a";

  pinMode(pin, INPUT);
  data += String(pin) + String(analogRead(pin));
  webSocketServer.sendData(data);
}

void setup() {


  Serial.begin(9600);


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // This delay is needed to let the WiFly respond properly
  delay(100);
}

void loop() {
  String data;
  WiFiClient client = server.available();

  if (webSocketServer.handshake(client)) {

    while (client.connected()) {
      data = webSocketServer.getData();

      if (data.length() > 0) {
        handleClientData(data);
      }

      sendClientData(1);
      sendClientData(2);
      sendClientData(3);
      delay(1);
    }
    delay(1);
    Serial.println("connection closed");
  }
  
  // wait to fully let the client disconnect
  delay(100);
}