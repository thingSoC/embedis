#include "embedis.h"

void setup() {
  Serial.begin(57600);
}

void loop() {
  int b = Serial.read();
  if (b >= 0) embedis_in(b);  
}

void embedis_out(char b) {
  Serial.write(b);
}
