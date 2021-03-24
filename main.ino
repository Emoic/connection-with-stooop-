#include <Hash.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>


const char* ssid = "403B";
const char* password = "swjtumaker144";
//RC522引脚定义
const int pin_rst = 0;
const int pin_ss = 2;
MFRC522 card(pin_ss,pin_rst);
//uid允许最小长度
const int authorized_length=4;

String uid = "";


bool wifi_connect(const char* ssid,const char* password);
bool wifi_check();
void dump_byte_array(byte *buffer, byte bufferSize);
String get_card(byte *buffer, byte bufferSize);



void setup() {
    Serial.begin(115200);
    SPI.begin();
    delay(100);
    card.PCD_Init();
    delay(500);

    wifi_connect(ssid,password);
}

void loop() {
      //检查WiFi是否连接上
    if (!wifi_check)
        return;

    if(card.PICC_IsNewCardPresent()){
        if(card.PICC_ReadCardSerial()){
            dump_byte_array(card.uid.uidByte, card.uid.size);
            Serial.print("uid=");
            uid=get_card(card.uid.uidByte, card.uid.size);
            Serial.println(uid);
            card.PICC_HaltA();
        }
    }
}





bool wifi_connect(const char* ssid,const char* password){
    int count=1;
    Serial.print("connecting to:");
    Serial.println(ssid);
    //初始化
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid,password);
    //连接，尝试次数30次
    while(WiFi.status()!=WL_CONNECTED && count<30){
        delay(1000);
        count++;
        Serial.print(".");
  }
  //最后检查是否连接上
  if(WiFi.status()==WL_CONNECTED){
    Serial.print(ssid);
    Serial.println(" conected");
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
    return true;
  }
  else{
    Serial.print(ssid);
    Serial.print("conect failed!");
    Serial.println("check your password!!!");
    return false;
  }
}

//检查是否连接上
bool wifi_check(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("try to connect wifi again");
    if(!wifi_connect(ssid,password)){
      return false;
    }
    else{
      return true;
    }
  }
  else{
    return true;
  }
}

//uid 熟悉形式
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; ++i) {
    Serial.print(buffer[i] < 100 ? buffer[i] < 10 ? " 00" : " 0" : " ");
    Serial.print(buffer[i]);
  }
  Serial.println("");
}

String get_card(byte *buffer, byte bufferSize){
  //检查UID长度是否合适
  if(bufferSize < authorized_length)
  {
    Serial.println("UID is too short");
    return "";
  }
  String uid = "";
  for (byte i = 0; i < bufferSize; ++i) {
    uid += buffer[i];
  }
  return sha1(uid);
}
