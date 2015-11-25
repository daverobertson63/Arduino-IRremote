/*
 
Based on IRSend demo from ken Shirriffs library - this sends simple commands to a RoboSapien V1 using
and IR Transmitter with Arduino
 
*/
 
#include <IRremote.h>
 
IRsend irsend;
IRrecv irrecv(11);
 
void setup()
{
Serial.begin(9600);
}
 
void loop() {
 
irsend.sendRSV1(142);
delay(1000);
 
if (Serial.read() != -1) {
 
Serial.println("started");
irsend.sendRSV1(129);
irsend.sendRSV1(199);
 
/*
irsend.sendRSV1(129);
irsend.sendRSV1(129);
delay(1000);
irsend.sendRSV1(169);
delay(2000);
irsend.sendRSV1(162);
delay(2000);
irsend.sendRSV1(173);
delay(2000);
irsend.sendRSV1(196);
delay(2000);
irsend.sendRSV1(206);
delay(2000);
irsend.sendRSV1(199);
delay(2000);
Serial.println("Done");
*/
 
}
 
}