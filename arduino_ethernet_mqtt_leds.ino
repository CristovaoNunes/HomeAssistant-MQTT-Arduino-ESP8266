/*
 Example MQTT-switch-relay-node with 4 buttons and 4 leds 
 
  - connects to an MQTT server
  - publishes "hello world" to the  "led"
  - subscribes to the  "led"
  - controls 4 leds on pins 2,3,5 and 6 - leds can be replaced with relays
  - reads 4 button on pins 7,8,9 and 10
  - turns on/off a specific led when it receives a specific "on"/"off" from the "led" 
  - sends a specific "on"/"off" to the "led" topic a specific button is pressed
  - multiple arduino's with same generic sketch can run parallel to each other
  - multiple arduino's need each to have a unique ip-addres, unique mac address and unique MQTT client-ID

  - tested on arduino-uno with W5100 ethernet shield
  - Ethernet Shield W5100 uses pins 4,10,11,12,13
  - availbale digital pins: 1,2,3,5,6,7,8,9,10 
*/

//------------------------------------------------------------------------------

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Bounce2.h>

int led1 = 2;
int button1 = 7;
int led1Value = LOW; 

int led2 = 3;
int button2 = 8;
int led2Value = LOW; 

byte mac[]    = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
byte ip[] = { 192, 168, 1, 102 }; 

byte server[] = { 192, 168, 1, 101 };

// Handle and convert incoming MQTT messages ----------------------------------------

void callback(char* , byte* payload, unsigned int length) {
  String content="";
  char character;
  for (int num=0;num<length;num++) {
      character = payload[num];
      content.concat(character);
  }   
  Serial.println();
  Serial.println(content); 
  
  if (content == "sala_luz_teto_on") {
    led1Value = HIGH;    
  }
  
  if (content == "sala_luz_teto_off") {
    led1Value = LOW;
  }

  if (content == "sala_luz_tv_on") {
    led2Value = HIGH;    
  }
  
  if (content == "sala_luz_tv_off") {
    led2Value = LOW;
  }
  
  digitalWrite(led1,led1Value);
  digitalWrite(led2,led2Value);

}

// Initiate instances -----------------------------------

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Initiate a bouncer instance for each button
Bounce bouncer1 = Bounce();
Bounce bouncer2 = Bounce();

//-------------------------------------------------------

void setup()

{

  // setup led, button, bouncer 1 -----------------------
  pinMode(led1, OUTPUT);
  pinMode(button1,INPUT);
  digitalWrite(button1,HIGH);
  bouncer1 .attach(button1);
  bouncer1 .interval(5);

  pinMode(led2, OUTPUT);
  pinMode(button2,INPUT);
  digitalWrite(button2,HIGH);
  bouncer2 .attach(button2);
  bouncer2 .interval(5);

  Serial.begin(9600);

 
  Ethernet.begin(mac, ip);
  if (client.connect("arduino-ip-238")) {
    //client.publish("led","hello world - here arduino ip 239");
    Serial.println("connected");
    client.subscribe("home"); 
  }
}

//----------------------------------------------

void loop()
{// Listen for button interactions and take actions ----------------------------------------  
// Note: Button actions do send MQTT message AND do set led(x)Value to HIGH or LOW

  if (bouncer1.update()) {
    if (bouncer1.read() == HIGH) {
      if (led1Value == LOW) {
        //led1Value = HIGH;
        client.publish("home","sala_luz_teto_on");
        Serial.println("led ligado");              
      } else {
        client.publish("home","sala_luz_teto_off");
        //led1Value = LOW;
        Serial.println("led desligado");
      }
    }
  }

  if (bouncer2.update()) {
    if (bouncer2.read() == HIGH) {
      if (led2Value == LOW) {
        //led1Value = HIGH;
        client.publish("home","sala_luz_tv_on");
        Serial.println("led ligado");              
      } else {
        client.publish("home","sala_luz_tv_off");
        //led1Value = LOW;
        Serial.println("led desligado");
      }
    }
  }  

  
//------------------------------------------------  

  digitalWrite(led1,led1Value);
  digitalWrite(led2,led2Value);
  
  client.loop();
}

// End of sketch ---------------------------------

