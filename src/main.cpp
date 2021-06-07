#include <ESP8266WiFi.h>
#include <MicroGear.h>
#define APPID   "NETPIE2VALVE"
#define KEY     "<KEY>"
#define SECRET  "<SECRET>"
#define ALIAS   "Relay_valve" 

WiFiClient client;
MicroGear microgear(client);
const char* ssid = "XXXXX"; 
const char* password = "XXXXX";
const char* wifi_Check_status = "";
long timer = 0;
bool reconnect = false;

/* ฟังก์ชันสำหรับอ่านค่า json จาก api (เรียกใช้ใน infinity loop) */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    String iMsg = "";
      for (int i = 0; i < msglen; i++) {
        iMsg += (char)msg[i];
      }
  if(iMsg == "ON1" || iMsg == "ON2" || iMsg == "ONALL" || iMsg == "OFFALL"){
    if (iMsg == "ON1") 
    {
        Serial.println("ON1."); // ส่งข้อความไปยังบอร์ดอาร์ดูโน่แบบ serial communication ผ่านserial portสาย tx/rx 
    } 
      if (iMsg == "ON2") {
        Serial.println("ON2."); // ส่งข้อความไปยังบอร์ดอาร์ดูโน่แบบ serial communication ผ่านserial portสาย tx/rx 
      }
      if (iMsg == "ONALL") {
        Serial.println("ONALL."); // ส่งข้อความไปยังบอร์ดอาร์ดูโน่แบบ serial communication ผ่านserial portสาย tx/rx 
      }
      if (iMsg == "OFFALL") {
        Serial.println("OFFALL."); // ส่งข้อความไปยังบอร์ดอาร์ดูโน่แบบ serial communication ผ่านserial portสาย tx/rx 
      }
  }
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    microgear.setAlias(ALIAS);
}

void setup() {
      Serial.begin(115200); // เลือกค่า Buad Rate สำหรับการสื่อสารแบบ Serial Communication ผ่านserial portสาย tx/rx 
      WiFi.disconnect(); // Disconnect AP
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);   //Connect WIFI

      while (WiFi.status() != WL_CONNECTED) // ดีเลย์ 500 วินาที เพื่อรอการเชื่อมต่อ WiFi
      {
        delay(500);
      }
      microgear.on(MESSAGE,onMsghandler);
      microgear.on(CONNECTED,onConnected);
      microgear.init(KEY,SECRET,ALIAS);
      microgear.connect(APPID);
      wifi_Check_status = "connect";
      Serial.println("connect."); // ส่งข้อความไปยังบอร์ดอาร์ดูโน่แบบ serial communication ผ่านserial portสาย tx/rx 
}

void loop() {  // Infinity loop ที่จะทำงานตลอดเวลาที่ตัวบอร์ดเปิดอยู่
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
              microgear.subscribe("/relaystat");    // เลือกแชแนลที่จะส่งข้อความชื่อ relaystat
              microgear.publish("/relaystat","1",true); //postค่าสถานะของรีเลย์ไปที่ api 
            }
            if(msg == "OP2")  //valve1 = OFF - Valve2 = ON
            {   
              microgear.subscribe("/relaystat"); // เลือกแชแนลที่จะส่งข้อความชื่อ relaystat
              microgear.publish("/relaystat","2",true); //postค่าสถานะของรีเลย์ไปที่ api 
            }
            if(msg == "ED3")  //valve1 = ON - Valve2 = ON
            {   
              microgear.subscribe("/relaystat"); // เลือกแชแนลที่จะส่งข้อความชื่อ relaystat
              microgear.publish("/relaystat","3",true); //postค่าสถานะของรีเลย์ไปที่ api 
            }
            if(msg == "ED4")  //valve1 = OFF - Valve2 = OFF
            {   
              microgear.subscribe("/relaystat"); // เลือกแชแนลที่จะส่งข้อความชื่อ relaystat
              microgear.publish("/relaystat","4",true); //postค่าสถานะของรีเลย์ไปที่ api 
            }
    }
}
