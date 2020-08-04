#include <Sensitive.h>


/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

/*Tushar Saurabh
 * 
 * Soil Moisture: Capacitative
 * Soil Temperature:
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS 4


// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

String SENSOR = String("SENSOR_1");

//Sensors attached
bool tempSensor = false;
bool capacitative = false; //capacitative soil temperature
bool dsbSensor = true; //ds18b20

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

int soilPin = A0;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String payload = String("sensor:");
  payload += SENSOR;
  String soilpayload = String(",attr:soil,unit:%,value:");
  String tempPayload = String(",attr:temp,unit:C,value:");
  String humidPayload = String(",attr:hmd,unit:%,value:");
  String soilTempPayload = String(",attr:sotm,unit:C,value:");
  
  unsigned long now = millis();
  
  if (now - lastMsg > 5000) {
    String temp = String();
    lastMsg = now;

    /*Air: 850
     * Water: 440
     */
    if (capacitative){
      int val = analogRead(soilPin);
      Serial.print("Analog Reader read: ");
      Serial.print(val);
      Serial.print(" ");
      int absVal = abs(850 - val);
      float percent = (absVal/410.0)*100.0;
      Serial.print(percent);
      Serial.print("\n");
    }

    if (dsbSensor){
      sensors.requestTemperatures(); 

      //print the temperature in Celsius
      Serial.print("Temperature: ");
      Serial.print(sensors.getTempCByIndex(0));
      Serial.print((char)176);//shows degrees character
      Serial.print("C  |  ");
      
      //print the temperature in Fahrenheit
      Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
      Serial.print((char)176);//shows degrees character
      Serial.println("F");
    }

    delay(3000);

    /*
    char c[10];
    sprintf(c,"%d",soilMoisture);

    temp = payload + soilpayload + c;
    char charBuf[100];
    temp.toCharArray(charBuf, 100) ;
    
    client.publish("sensor",charBuf );

    delay(3000);

    
      
      char t[10];
      sprintf(t,"%d",(int)temperature);
      temp = payload + tempPayload + t;
      memset(charBuf,0,sizeof(charBuf));
      
      temp.toCharArray(charBuf, 100) ;
      client.publish("sensor",charBuf );
  
      delay(3000);
  
      char h[10];
      sprintf(h,"%d",(int)humidity);
      temp = payload + humidPayload + h;
      memset(charBuf,0,sizeof(charBuf));
      temp.toCharArray(charBuf, 100) ;
      client.publish("sensor",charBuf );
  
      delay(3000);
    }*/
    
    /*Serial.println("I'm awake, but I'm going into deep sleep mode for 60 seconds\n");
    //ESP.deepSleep(1800e6); 
    ESP.deepSleep(60e6); */
    
  }
}
