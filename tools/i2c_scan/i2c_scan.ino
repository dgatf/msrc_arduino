#include <Wire.h>

void setup() {
  Serial.begin(19200);
  Serial.println("I2C scanner");
  Wire.begin();
  Wire.setTimeout(1);
}

void loop() {
  uint8_t error, address;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission(true);
    if (error == 0)
    {
      Serial.println(address, HEX);
    }
  }
}
