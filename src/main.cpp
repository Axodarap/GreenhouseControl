#include <Arduino.h>
#include "Multiplexer.h"

Multiplexer multiplexer(4, 5, 6, 7, A0);

void setup() {
  Serial.begin(9600);
  multiplexer.Init();
}

void loop() {
  
for(int i=1; i<17; i++){
  Serial.print((String)multiplexer.ReadValue(i) + "\t");  
 }
 Serial.println();
 delay(500);
}