#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "helpers.h"
#include "global.h"

#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_Information.h"
#include "PAGE_NetworkConfiguration.h"
#include "Page_Logging.h"

// Parameters for temporary AP on boot.
#define ACCESS_POINT_NAME  "ESPSwitch"        
#define ACCESS_POINT_PASSWORD  "12345678" 

WiFiClient espClient;


long previousMillis = 0;     // will store last time loop ran
long interval = 500;           // interval at which to limit loop
int loopCount = 0;             // used to run temp update every 10 seconds
int i;
unsigned long bootTime = 0;
int lvPowerPin = 0;
int hvPowerPin = 2;

byte z0;
byte z1;
byte z2;
byte z3;
byte x0;
byte x1;
byte x2;
byte x3;
byte x4;
byte intra = 1;
byte inter = 7;


//MQTT
PubSubClient MQTTClient(espClient);
// The MQTT path to subscribe to - change as you wish but must be null terminated i.e with '\0'
// char subPath[] = {'h', 'a', '/', 'm', 'o', 'd', '/', 'F', 'F', 'F', 'F', '/', '#','\0'};
//See below           0    1    2    3    4    5    6    7    8    9    10   11   12  

char subPath[] = {'h', 'a', '/', 'm', 'o', 'd', '/', 'F', 'F', 'F', 'F', '/', '#','\0'};

// The character in the MQTT topic string representing the start of commands
// The value depends on the topic you are subscribing to (Zero indexed)
byte topicNumber = 12;

//*******************************
//      FUNCTIONS

void ConfigureWifi()
{
 //Serial.println(ESP.getChipId());
 //Serial.print("MQTT ID: ");
  //Serial.println("Configuring Wifi");
  WiFi.begin (config.ssid.c_str(), config.password.c_str());
  if (!AdminEnabled){
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //Serial.print(".");
    }
    //Serial.println("");
    //Serial.println("WiFi connected");
  }
  
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
  }
 //Serial.print("IP address: ");
 //Serial.println(WiFi.localIP());
 //Serial.print("Serial: ");
  
 unsigned long espChipId = ESP.getChipId();
 char b[7];
 sprintf(b, "%i", espChipId);
 for (i=0;i<4;i++){
  id[i] = b[i+3];
 }
 //Serial.println(id);

 // Redefine the ID path
 for (i=0;i<4;i++){
   subPath[i+7] = id[i];
 }
   //subPath[] = {'h', 'a', '/', 'm', 'o', 'd', '/', id[0], id[1], id[2], id[3], '/', '#','\0'};

}

// fromCoffeemaker receives a 4 byte UART package from the coffeemaker and translates them to a single ASCII byte
byte fromCoffeemaker(byte x0, byte x1, byte x2, byte x3){
  // Print received UART bytes on console
      //Serial.write(x0);
      //Serial.write(x1);
      //Serial.write(x2);
      //Serial.write(x3);
      //Serial.println();
  
  // Reads coding Bits of the 4 byte package and writes them into a new character (translates to ASCII)
    bitWrite(x4, 0, bitRead(x0,2));
    bitWrite(x4, 1, bitRead(x0,5));
    bitWrite(x4, 2, bitRead(x1,2));
    bitWrite(x4, 3, bitRead(x1,5));
    bitWrite(x4, 4, bitRead(x2,2));
    bitWrite(x4, 5, bitRead(x2,5));
    bitWrite(x4, 6, bitRead(x3,2));
    bitWrite(x4, 7, bitRead(x3,5));
  // Print translated ASCII character
  //  Serial.println(char(x4));
}

// toCoffeemaker translates an ASCII character to 4 UART bytes and sends them to the coffeemaker
byte toCoffeemaker(byte zeichen)
{
  z0 = 255;
  z1 = 255;
  z2 = 255;
  z3 = 255;
// Reads bits of ASCII byte and writes it into coding bits of 4 UART bytes  
  bitWrite(z0, 2, bitRead(zeichen,0));
  bitWrite(z0, 5, bitRead(zeichen,1));
  bitWrite(z1, 2, bitRead(zeichen,2));  
  bitWrite(z1, 5, bitRead(zeichen,3));
  bitWrite(z2, 2, bitRead(zeichen,4));
  bitWrite(z2, 5, bitRead(zeichen,5));
  bitWrite(z3, 2, bitRead(zeichen,6));  
  bitWrite(z3, 5, bitRead(zeichen,7)); 
  
// Prints hex and bin values of translated UART bytes and the source ASCII character 
    //Serial.print(z0, HEX); //Serial.print(" ");
    //Serial.print(z1, HEX); Serial.print(" ");
    //Serial.print(z2, HEX); Serial.print(" ");
    //Serial.print(z3, HEX); Serial.print("\t");

    //Serial.print(z0, BIN); Serial.print(" ");
    //Serial.print(z1, BIN); Serial.print(" ");
    //Serial.print(z2, BIN); Serial.print(" ");
    //Serial.print(z3, BIN); Serial.print("\t");
//    
    //Serial.write(zeichen);
    //Serial.println();

// Sends a 4 byte package to the coffeemaker
    delay (intra); Serial.write(z0);
    delay (intra); Serial.write(z1);
    delay (intra); Serial.write(z2);
    delay (intra); Serial.write(z3); 
    delay(inter);   
}

void callback(char* topic, byte* payload,unsigned int length) {
//  char message_buff[6];
//  int x = 0;
//  
//  // create character buffer with ending null terminator (string)
//  for(x=0; x<length; x++) {
//    message_buff[x] = payload[x];
//  }
   
//  message_buff[x] = '\0';
//  int newState = atoi(message_buff);
//  Serial.print("R:");
//  Serial.print(topic);
//  Serial.print("/");
//  Serial.println(newState);
  
  if (length < 2){
    //toCoffeemaker(payload[0]);
    if (payload[0] == 1 || payload[0] == 0 || payload[0] == 48 || payload[0] == 49){
      digitalWrite(lvPowerPin, LOW);
      delay(50);
      digitalWrite(hvPowerPin, LOW);
      delay(200);
      digitalWrite(hvPowerPin, HIGH);
      digitalWrite(lvPowerPin, HIGH);
    }
  }else{

    for (int x=0;x < length; x++){
      toCoffeemaker(payload[x]);
    }
      toCoffeemaker(0x0D);
      toCoffeemaker(0x0A);
  }
}

void publishTopicValue(char* strString, char* value) {
  //Serial.print("S:");
  //Serial.print(strString);
  //Serial.print("/");
  //Serial.println(value);
  MQTTClient.publish(strString, value);
}


void mqttConnect() {
  if (!MQTTClient.connected()) {
    //Serial.println("Connecting to broker");
    if (MQTTClient.connect(id)){
      //Serial.print("Subscribing to :");
      //Serial.println(subPath);
      MQTTClient.subscribe(subPath);
    }else{
      //Serial.println("Failed to connect!");
    }
  }
}



//***********************************************

void setup() {
  EEPROM.begin(512);
  Serial.begin(9600);

  //Init the relay pins
  pinMode(lvPowerPin, OUTPUT);
  pinMode(hvPowerPin, OUTPUT);
  digitalWrite(lvPowerPin, HIGH);
  digitalWrite(hvPowerPin, HIGH);

  
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "MYSSID";
    config.password = "MYPASSWORD";
    config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.mqttserver = "192.168.0.253";
    config.hostname = "Coffee Machine";
    WriteConfig();
    //Serial.println("General config applied");
  }

  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);  // AP mode
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    //Serial.print("AP IP address: ");
    //Serial.println(myIP);
  }
  else
  {
    WiFi.mode(WIFI_STA); // Client Mode
    ConfigureWifi();
  }
  
  char *mqtt_server = &config.mqttserver[0u]; //Convert the server string to char array for mqtt client
  
  MQTTClient.setServer(mqtt_server, 1883);
  MQTTClient.setCallback(callback);

  server.on ( "/favicon.ico",   []() { server.send ( 200, "text/html", "" );   }  );
  server.on ( "/", []() { server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin.html", []() { server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/logging.html", send_logging_html );
  server.on ( "/style.css", []() { server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/logging", send_logging_values_html );

  server.onNotFound ( []() { server.send ( 400, "text/html", "Page not Found" );   }  );
  server.begin();
  //Serial.println( "HTTP server started" );
}

void loop() {
    if (AdminEnabled) { //Is Admin / AP Mode enabled?
      if (millis() > AdminTimeOut) {
      //if (0 > 1){
        AdminEnabled = false;
        //Serial.println("Admin Mode disabled!");
        WiFi.mode(WIFI_STA); // Put back to client mode.
        ConfigureWifi(); // And reconnect
      }
    }else{
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      
      // Make sure we are connected
      if (WiFi.status() != WL_CONNECTED) {
        ConfigureWifi();
      }
      mqttConnect();
    }
    
    //Coffee Machine commands
//    if(Serial.available()){       
//      byte c = Serial.read();
//        toCoffeemaker(c);
//    }
      byte d0; byte d1; byte d2; byte d3;
      while(Serial.available()){
        delay (intra); byte d0 = Serial.read();
        delay (intra); byte d1 = Serial.read();
        delay (intra); byte d2 = Serial.read();
        delay (intra); byte d3 = Serial.read();
        delay (inter);
              
        // Print hex and bin values of received UART bytes
//        Serial.print(d0, HEX); Serial.print(" ");
//        Serial.print(d1, HEX); Serial.print(" ");
//        Serial.print(d2, HEX); Serial.print(" ");
//        Serial.print(d3, HEX); Serial.print("\t");
//        
//        Serial.print(d0, BIN); Serial.print(" ");
//        Serial.print(d1, BIN); Serial.print(" ");
//        Serial.print(d2, BIN); Serial.print(" ");
//        Serial.print(d3, BIN); Serial.print("\t");
        fromCoffeemaker(d0,d1,d2,d3);
      } 
    MQTTClient.loop();
  }
  server.handleClient();
}
