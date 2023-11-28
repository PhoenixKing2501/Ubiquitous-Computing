///*
// * Rui Santos 
// * Complete Project Details https://randomnerdtutorials.com
// */
// 
//#include <TinyGPSPlus.h>
//#include <SoftwareSerial.h>
//
//static const int RXPin = 4, TXPin = 3;
//static const uint32_t GPSBaud = 9600;
//
//// The TinyGPS++ object
//TinyGPSPlus gps;
//
//// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
//  
//void setup(){
//  Serial.begin(9600);
//  ss.begin(GPSBaud);
//}
//
//void loop(){
//  // This sketch displays information every time a new sentence is correctly encoded.
//  while (ss.available() > 0){
//    
//    gps.encode(ss.read());
//    Serial.println(
//    if (gps.location.isUpdated()){
//      Serial.println("Here");
//      Serial.print("Latitude= "); 
//      Serial.print(gps.location.lat(), 6);
//      Serial.print(" Longitude= "); 
//      Serial.println(gps.location.lng(), 6);
//    }
//  }
//}

#include <SoftwareSerial.h>

int Rx_pin = 7;
int Tx_pin = 6;
SoftwareSerial SerialGPS(Rx_pin, Tx_pin);

void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
}

void loop()
{
  while (SerialGPS.available() > 0)
  Serial.write(SerialGPS.read());
}