#include <ESP8266WiFi.h>
#include <MicroGear.h>
#define APPID   "NETPIE2VALVE"
#define KEY     "WcTxK4EMocRJCcF"
#define SECRET  "H0AHhsFat0L0AIBmdmR3IhN6J"
#define ALIAS   "Relay_valve" 

WiFiClient client;
MicroGear microgear(client);
const char* ssid = "Wanna_2.4G_plus"; //3bb-Aekkaphob_plus
const char* password = "0632017824"; //0927569471
const char* wifi_Check_status = "";
long timer = 0;
bool reconnect = false;

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    String iMsg = "";
      for (int i = 0; i < msglen; i++) {
        iMsg += (char)msg[i];
      }
  if(iMsg == "ON1" || iMsg == "ON2" || iMsg == "ONALL" || iMsg == "OFFALL"){
    if (iMsg == "ON1") 
    {
        Serial.println("ON1.");
    } 
      if (iMsg == "ON2") {
        Serial.println("ON2.");
      }
      if (iMsg == "ONALL") {
        Serial.println("ONALL.");
      }
      if (iMsg == "OFFALL") {
        Serial.println("OFFALL.");
      }
  }
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    microgear.setAlias(ALIAS);
}

void setup() {
      Serial.begin(115200);
      WiFi.disconnect(); // Disconnect AP
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);   //Connect WIFI

      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
      }
      microgear.on(MESSAGE,onMsghandler);
      microgear.on(CONNECTED,onConnected);
      microgear.init(KEY,SECRET,ALIAS);
      microgear.connect(APPID);
      wifi_Check_status = "connect";
      Serial.println("connect.");
}

void loop() {
      if(WiFi.status() != WL_CONNECTED)
      {
        if(!wifi_Check_status || wifi_Check_status == "connect")
        {
          wifi_Check_status = "lost";
          Serial.println("lost.");
        }
      }
      if(WiFi.status() == WL_CONNECTED)
      {
        if(!wifi_Check_status || wifi_Check_status == "lost")
        {
          wifi_Check_status = "connect";
          Serial.println("connect.");
        }
      }
    /* To check if the microgear is still connected */
    if (microgear.connected()) 
    {
        microgear.loop();
    }
    if (microgear.connected() && !reconnect) 
    {
        reconnect = true;
    }
    else if (!microgear.connected() && reconnect) 
    {
        Serial.println("OFFALL.");
        //if ((millis() - timer) % 500 == 0){}
        if ((millis() - timer) > 5000)
        {
          reconnect = false;
        }
    }
    else if (!microgear.connected() && !reconnect) 
    {
        Serial.println("OFFALL.");
        microgear.connect(APPID);
        timer = millis();
    }
    delay(100);
    if (Serial.available()) 
    {
          String msg = Serial.readStringUntil('.');  //read serial messege character until '.'(dot)
            if(msg == "OP1")  //valve1 = ON - Valve2 = OFF
            {   
              microgear.subscribe("/relaystat");
              microgear.publish("/relaystat","1",true);
            }
            if(msg == "OP2")  //valve1 = OFF - Valve2 = ON
            {   
              microgear.subscribe("/relaystat");
              microgear.publish("/relaystat","2",true);
            }
            if(msg == "ED3")  //valve1 = ON - Valve2 = ON
            {   
              microgear.subscribe("/relaystat");
              microgear.publish("/relaystat","3",true);
            }
            if(msg == "ED4")  //valve1 = OFF - Valve2 = OFF
            {   
              microgear.subscribe("/relaystat");
              microgear.publish("/relaystat","4",true);
            }
    }
}