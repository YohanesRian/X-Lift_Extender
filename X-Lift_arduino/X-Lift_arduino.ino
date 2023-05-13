#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

//Pin
#define SS_PIN        15      // D8 NodeMCU
#define RST_PIN       16      // D0 NodeMCU
const int led_rgb[] = {3, 9, 10};//r, g, b
const int multiplexer_1[] = {0, 4, 5};//0, 1, 2
const int relay_gnd = 2;

//RFID
MFRC522 rfid(SS_PIN, RST_PIN); 

//WiFi Connection
const char* WiFi_Name = "Yohanes"; 
const char* Pass = "yombohlho";

//Database 
const char *Host = "http://192.168.240.123/";


void setup() {
  Serial.begin(9600);
  Connect_to_Wifi();
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
  Set_Pin_Output(led_rgb);
  Set_Pin_Output(multiplexer_1);
  pinMode(relay_gnd, OUTPUT);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
      String CardID = Read_RFID();
      Access_Role(CardID);
    }
  }
  else{
    color_led("0");
    Connect_to_Wifi();
  }
  delay(10);
}

void Connect_to_Wifi(){
  WiFi.begin(WiFi_Name, Pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  color_led("blue");
}

void Set_Pin_Output(const int *pin){
  for(int i = 0; i < (sizeof(pin)/sizeof(pin[0])); i++){
    pinMode(pin[i], OUTPUT);
  }
}

String Read_RFID(){
  byte *data = rfid.uid.uidByte;
  byte data_size = rfid.uid.size;

  String result = "";
  
  for (byte i = 0; i < data_size; i++){
    if(i != 0){
      result += String(data[i] < 0x10 ? "0" : ":");    
    }
    result += String(data[i], HEX);
    result.toUpperCase();
  }
   return result;
}

String Get_Role(String CardID){
  HTTPClient http;
  WiFiClient client;
  String Role = "";
  String Address = "X-Lift/Get_Role.php?CardID="+ CardID;
  String Link = Host + Address;
  http.begin(client, Link);
  int httpResponseCode = http.GET(); 
  if(httpResponseCode > 0){
    Role = http.getString();
  }
  http.end();
  return Role;
}

int Get_Floor(String CardID){
  HTTPClient http;
  WiFiClient client;
  int Floor_Number = -1;
  String Floor = "";
  String Address = "X-Lift/Get_Floor.php?CardID="+ CardID;
  String Link = Host + Address;
  http.begin(client, Link);
  int httpResponseCode = http.GET(); 
  if(httpResponseCode > 0){
    Floor = http.getString();
  }
  http.end();
  
  if(!Floor.equalsIgnoreCase("") && !Floor.equalsIgnoreCase("NONE")){
    int Floor_Number = -1;
    int i = 1;
    while(true){
      String number = i + "";
      if(Floor.equals(number)){
        Floor_Number = i;
        break;
      }
      i++;
    } 
  }
  return Floor_Number;
}

void Access_Role(String CardID){
  String Role = Get_Role(CardID);
  if(Role.equalsIgnoreCase("staff")){
    color_led("green");
    digitalWrite(relay_gnd, HIGH);
    delay(5000);
    color_led("blue");
    digitalWrite(relay_gnd, LOW);
  }
  else if(Role.equalsIgnoreCase("customer")){
    Access_Floor(CardID);
  }
  else if(Role.equalsIgnoreCase("")){}
  else{
    color_led("red");
    delay(3000);
    color_led("blue");
  }
}

void Access_Floor(String CardID){
  int Floor = Get_Floor(CardID);
  if(Floor == -1){
    color_led("green");
    delay(5000);
    color_led("blue");
  }
  else{
    if(Floor <= 7 && Floor > 0){
      int condition[3];
      for(int i = 0; i < 3; i++){
        condition[i] = (Floor % 2 == 0 ? LOW : HIGH);
        if(Floor % 2 == 1){
          Floor = Floor - 1;
        }
        Floor = Floor / 2;
      }
      turn_multiplexer(condition);
      digitalWrite(relay_gnd, HIGH);
      delay(100);
      digitalWrite(relay_gnd, LOW);
      condition[0] = condition[1] = condition[2] = 0;
      turn_multiplexer(condition);
    }
  }
}

void color_led(String color){
  int condition[3];
  if(color.equals("red")){
      condition[0] = HIGH;
      condition[1] = LOW;
      condition[2] = LOW;
  }
  else if(color.equals("green")){
      condition[0] = LOW;
      condition[1] = HIGH;
      condition[2] = LOW;
  }
  else if(color.equals("blue")){
      condition[0] = LOW;
      condition[1] = LOW;
      condition[2] = HIGH;
  }
  else{
      condition[0] = LOW;
      condition[1] = LOW;
      condition[2] = LOW;
  }
  turn_led(condition);
}

void turn_led(int *condition){
  for(int i=0; i<3; i++){
    digitalWrite(led_rgb[i], condition[i]);
  }
}
void turn_multiplexer(int *condition){
  for(int i=0; i<3; i++){
    digitalWrite(multiplexer_1[i], condition[i]);
  }
}
