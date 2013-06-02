#include <SPI.h>
#include <WiFi.h>
#include <dht11.h>

dht11 DHT11;

#define DHT11PIN 3

#define SERIAL

char ssid[] = "Tukaha Jiu Jitsu"; //  your network SSID (name) 
char pass[] = "boomtown";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(173,194,73,105);  // numeric IP for Google (no DNS)
char server[] = "gardenmonitor.appspot.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  digitalWrite(13, HIGH);   // set the LED on
  
  #ifdef SERIAL
  Serial.begin(9600);
  #endif
  
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    printLnSerial("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
}

void loop() {
  
  if (status != WL_CONNECTED) {
    connectWifi();
  }
  
  getReading();
  String message = buildMessage();
  printLnSerial(message);
  sendMessage(message);
  recieveMessage();
  
  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);
  digitalWrite(13, LOW);   // set the LED off
  delay(29000);
}

void connectWifi()
{
  while ( status != WL_CONNECTED) { 
      printSerial("Attempting to connect to SSID: ");
      printLnSerial(ssid);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
    
      // wait 10 seconds for connection:
      delay(10000);
    }
    
  printWifiStatus();
}

void getReading()
{
  int chk = 1;
  while ( chk != 0 )
  {
    chk = DHT11.read(DHT11PIN);
  
    printSerial("Read sensor: ");
    switch (chk)
    {
      case DHTLIB_OK: 
                  printLnSerial("OK"); 
                  break;
      case DHTLIB_ERROR_CHECKSUM: 
                  printLnSerial("Checksum error"); 
                  break;
      case DHTLIB_ERROR_TIMEOUT: 
                  printLnSerial("Time out error"); 
                  break;
      default: 
                  printLnSerial("Unknown error"); 
                  break;
    }
  }
}

String buildMessage() 
{
  String message = String("");

  message += String("T:");
  message += String(DHT11.temperature, DEC);
  message += String(",H:");
  message += String(DHT11.humidity, DEC);

  return message;
}

void sendMessage(String message)
{
  printLnSerial("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    printLnSerial("connected to server");
    // Make a HTTP request:
    client.println("POST /sensorreceiver HTTP/1.1");
    client.println("Host:gardenmonitor.appspot.com");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(message.length());
    client.println();
    client.println(message);
  }
}

void recieveMessage()
{
  while (client.connected())
  {
    // if there are incoming bytes available 
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      #ifdef SERIAL
      Serial.write(c);
      #endif
    }
  }
  
  printLnSerial("");
  printLnSerial("disconnecting from server.");
  client.stop();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  
  IPAddress ip = WiFi.localIP();
  long rssi = WiFi.RSSI();
  
  #ifdef SERIAL
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address: 
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  
  #endif
  
  
}

void printSerial(String message)
{
  #ifdef SERIAL
  Serial.print(message);
  #endif
}

void printLnSerial(String message)
{
  #ifdef SERIAL
  Serial.println(message);
  #endif
}

void writeSerial(char c)
{
  #ifdef SERIAL
  Serial.write(c);
  #endif
}

