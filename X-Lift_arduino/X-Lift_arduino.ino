#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

//Pin
#define SS_PIN        15     // D8 
#define RST_PIN       16     // D0 
#define LED           9      // S3
#define RELAY_GND     1      // TX
const int Floor[] = {D1, D2, D3, D4};

//RFID
MFRC522 rfid(SS_PIN, RST_PIN); 

//WiFi Connection
const char* WiFi_Name = "Punya Yapson"; 
const char* Pass = "harusnyakamutau";

//Database 
const char *Host = "http://192.168.137.1/";

//Maximum floor
#define MAX_FLOOR 4

void setup(){
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(RELAY_GND, OUTPUT);
    pinMode(Floor[0], OUTPUT);
    pinMode(Floor[1], OUTPUT);
    pinMode(Floor[2], OUTPUT);
    pinMode(Floor[3], OUTPUT);
    reset_low();
    connect_wifi();
    SPI.begin();
    rfid.PCD_Init();
    rfid.PCD_SetAntennaGain(rfid.RxGain_max);
}

void loop(){
    digitalWrite(LED, HIGH);
    read_rfid();
    delay(100);
} 

void connect_wifi(){
    WiFi.hostname("X-Lift Extender");
    WiFi.begin(WiFi_Name, Pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    Serial.println("Connected");
}

void reset_low(){
    digitalWrite(LED, LOW);
    digitalWrite(RELAY_GND, LOW);
    digitalWrite(Floor[0], HIGH);
    digitalWrite(Floor[1], HIGH);
    digitalWrite(Floor[2], HIGH);
    digitalWrite(Floor[3], HIGH);
}

String get_rfid_uid(byte *buffer, byte bufferSize){
    String result = "";
  
    for (byte i = 0; i < bufferSize; i++){
        if(i != 0){
          result += String(buffer[i] < 0x10 ? "0" : ":");    
        }
        result += String(buffer[i], HEX);
        result.toUpperCase();
    }
    Serial.println("Card: " + result);
    
    return result;
}

void read_rfid(){
    if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
        String CardID = get_rfid_uid(rfid.uid.uidByte, rfid.uid.size);
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        search_access(CardID);
    }
}

String get_access(String CardID){
    HTTPClient http;
    WiFiClient client;
    String access = "";
    String Address = "X-Lift/Get_Access.php?CardID="+ CardID;
    String Link = Host + Address;
    http.begin(client, Link);
    int httpResponseCode = http.GET(); 
    if(httpResponseCode > 0){
      access = http.getString();
    }
    http.end();
    return access;
}

int get_number(String number){
    int result = -1;
    for(int i = 0; i <= MAX_FLOOR; i++){
      if(number.equals(String(i))){
        result = i;
        break;
      }
    }
    return result;
}

void search_access(String CardID){
    String access = get_access(CardID);
    if(access.equalsIgnoreCase("no")){
      Serial.println("You Have No Access");
      reset_low();
      delay(3000);
    }
    else if(access.equalsIgnoreCase("staff")){
      Serial.println("Staff");
      digitalWrite(RELAY_GND, HIGH);
      led_blink(10, 150);
      reset_low();
    }
    else{
      int number = get_number(access);
      if(number > 0 && number <= MAX_FLOOR){
        Serial.println("Floor: " + number);
        switch(number){
          case 1:
            digitalWrite(Floor[0], LOW);
            break;
          case 2:
            digitalWrite(Floor[1], LOW);
            break;
          case 3:
            digitalWrite(Floor[2], LOW);
            break;
          case 4:
            digitalWrite(Floor[3], LOW);
            break;
        }
        digitalWrite(RELAY_GND, HIGH);
        delay(100);
        reset_low();
        led_blink(number, 300);
      }
    }
}

void led_blink(int target, int tempo){
    for(int i = 0; i < target; i++){
      digitalWrite(LED, HIGH);
      delay(tempo);
      digitalWrite(LED, LOW);
      delay(tempo);
    }
}
