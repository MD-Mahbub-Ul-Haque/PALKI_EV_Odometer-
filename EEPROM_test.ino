#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512); // initialize EEPROM with size of 512 bytes
}

void loop() {
  int address = 0xFF;
  int readValue = EEPROM.read(address);
  Serial.println(readValue); // print value to serial monitor
  
  
  // write to EEPROM
  
  int value = 0;
  EEPROM.write(address, value);
  EEPROM.commit(); // save changes to EEPROM
  
//  // read from EEPROM
//  int readValue = EEPROM.read(address);
//  Serial.println(readValue); // print value to serial monitor
  
  delay(1000); // wait for 1 second before repeating
}
