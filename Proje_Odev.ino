/**********************************************************
 *      Nesnelerin İnterneti ve Uygulamaları Dersi
 *  Proje Uygulaması (Su sensörleriyle Seviye Kontrolü)
 *           Yazar : Sedanur SARIKOÇ
 *********************************************************/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <BlynkSimpleEsp8266.h>

char auth[] = "aepA4QbOE_ajromggLhcsrkl3BUAJyMQ"; //Blynk key

/*** Kablosuz Ağ Bilgileri ***/
const char* WLAN_SSID = "FiberHGW_ZTRY46_2.4GHz"; // "Kablosuz Ağ Adı" 
const char* WLAN_PASSWORD = "etzJfsRzRu"; // "Kablosuz Ağ Şifresi"

/*** ThingSpeak Kurulumu ***/
const char* thingSpeakHost = "api.thingspeak.com";
const char* writeAPIKey = "4PED2HUW23QBRVCB"; // thingSpeak api key
unsigned long channelID = 1256512;
const char* writeAPIKey2 = "21PIRRTTD7DFLRIB"; // thingSpeak api key
unsigned long channelID2 = 1264185;
unsigned  int field_no=1;

WiFiClient client;

char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "ZW0CU9G04ZHM67HV"; // thingTweet api key
int tweetSayac = 0;
int seviye1Kontrol1 = 0;
int seviye1Kontrol2 = 0;
int seviye2Kontrol1 = 0;
int seviye2Kontrol2 = 0;


void setup() {
  pinMode(D0, INPUT);
  Serial.begin(115200);

  WiFi_Setup();   //Kablosuz ağ bağlantı fonksiyonu
  ThingSpeak.begin(client);                 //  ThingSpeak client sınıfı  başlatılıyor
  Blynk.begin(auth, WLAN_SSID, WLAN_PASSWORD);  //Arduino Blynk'e bağlanıyor
  
}


/*** ESP8266 WiFi Kurulum Fonksiyonu ***/
void WiFi_Setup(){
  delay(10);
  Serial.println(); Serial.print(WLAN_SSID);
  Serial.print(" Kablosuz Agina Baglaniyor");
  WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
  //WiFi durum kontrolü
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();Serial.print(WLAN_SSID);
    Serial.println("Kablosuz Agina Baglandi");
    Serial.println("IP adresi: ");
    Serial.println(WiFi.localIP());

    
}

/*** Thingspeak Tweet Atma Fonksiyonu ***/
void Tweet_At(String  tweetData){
   if(client.connect (thingSpeakHost, 80)){
    tweetData = "api_key="+thingtweetAPIKey+"&status="+tweetData + " #" +tweetSayac;
            
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tweetData.length());
    client.print("\n\n");
    client.print(tweetData);

    Serial.println("Tweet atıldı.");
    tweetSayac++;
  }
  
}


void loop() {
  
  Blynk.run();
  
  WidgetLED led1(V0); //Blynk sanal pin ile led kaydedilir
  WidgetLED led2(V1); //Blynk sanal pin ile led kaydedilir

  
  if(digitalRead(D0) == false){ // sensör 1 su algılarsa
    analogWrite (D1, 500); //Donanımda led yakılır
    led1.on(); //Blynk led yakılır
    if(seviye1Kontrol1 == 0){ //sensör 1 durumu değiştiyse tweet at
      Tweet_At("Su seviyesi seviye 1'e ulaştı!");
      seviye1Kontrol1 = 1;
      seviye1Kontrol2 = 0;
      Blynk.notify("Su seviyesi seviye 1'e ulaştı!"); //Blynk telefonumuza bildirim gönderir
    }
    
  }
  else{ // sensör 1 su algılamazsa
    analogWrite (D1, 0); //Donanımda led söndür
    led1.off(); //Blynk led söndür
    if(seviye1Kontrol2 == 0){ //sensör 1 durumu değiştiyse tweet at
      Tweet_At("Su seviyesi seviye 1'in altında");
      seviye1Kontrol1 = 0;
      seviye1Kontrol2 = 1;
    }
    
  }

  delay(1000);
  if(analogRead(A0) >= 100){ // sensör 2 su algılarsa
    analogWrite (D2, 500); //Donanımda led yakılır
    led2.on(); //Blynk led yakılır
    if(seviye2Kontrol1 == 0){ //sensör 2 durumu değiştiyse tweet at
      Tweet_At("Su seviyesi seviye 2'ye ulaştı!");
      seviye2Kontrol1 = 1;
      seviye2Kontrol2 = 0;
      Blynk.notify("Su seviyesi seviye 2'ye ulaştı!"); //Blynk telefonumuza bildirim gönderir
    }
    
  }
  else{ // sensör 2 su algılamazsa
    analogWrite (D2, 0); //Donanımda led söndür
    led2.off(); //Blynk led söndür
    if(seviye2Kontrol2 == 0){ //sensör 2 durumu değiştiyse tweet at
      Tweet_At("Su seviyesi seviye 2'nin altında");
      seviye2Kontrol1 = 0;
      seviye2Kontrol2 = 1;
    }
  }


  
  if(client.connect (thingSpeakHost, 80)){ //ThingSpeak'e verileri gönder

    ThingSpeak.writeField (channelID, field_no, digitalRead(D0), writeAPIKey);
    ThingSpeak.writeField (channelID2, field_no, analogRead(A0), writeAPIKey2);
    Serial.println("\n");
  
  }

}
