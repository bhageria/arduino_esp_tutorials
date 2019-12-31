#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; //no need to edit here. Enter the SSID in the serial monitor
const char* password = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; //no need to edit here. Enter the password in the serial monitor
const char* mqtt_server = "io.adafruit.com";
#define mqtt_port 1883
#define MQTT_USER "USER"
#define MQTT_PASSWORD "PASSWORD"
#define MQTT_SERIAL_PUBLISH_CH "USER/feeds/trial"
#define MQTT_SERIAL_RECEIVER_CH "USER/feeds/ledBrightness"

WiFiClient wifiClient;

int wifiCredentials = 0;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "NodeMCUClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish(MQTT_SERIAL_PUBLISH_CH, "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_PUBLISH_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    //Serial.write(48);  
    Serial.write(payload, length);
    if(*payload == 'O' && *(payload+1)== 'N')
    {
      digitalWrite(13, HIGH);
    }
    else
    {
      digitalWrite(13, LOW);
    }
}

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.setTimeout(500);// Set time out for 
  getCredentials(); 

    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    delay(1000);
    reconnect();
  //setup_wifi();
  //client.setServer(mqtt_server, mqtt_port);
  //client.setCallback(callback);
  //reconnect();
  
}

void getCredentials()
{
    Serial.println("Enter the Wifi Name:");
    while(!Serial.available());
    char ssidbfr[50];
    memset(ssidbfr,0, 50);
    Serial.readBytesUntil( '\n',ssidbfr,50);
    ssid = ssidbfr;
    Serial.println(ssid);

    Serial.println("Enter the Wifi Password:");
    while(!Serial.available());
    char passbfr[50];
    memset(passbfr,0, 50);
    Serial.readBytesUntil( '\n',passbfr,50);
    password = passbfr;
    Serial.println(password);
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
   client.loop();
     if (Serial.available() > 0) {
     char bfr[101];
     memset(bfr,0, 101);
     Serial.readBytesUntil( '\n',bfr,100);
     publishSerialData(bfr);
   }
 }