#include <Wire.h>

const uint8_t SHT35_ADDR = 0x44;   // 0x45 si la broche ADDR est reliée à VCC

uint8_t crc8(const uint8_t *data, int len) {
  uint8_t crc = 0xFF;
  for (int i = 0; i < len; i++) {
    crc ^= data[i];
    for (int b = 0; b < 8; b++) {
      if (crc & 0x80) crc = (crc << 1) ^ 0x31;
      else crc <<= 1;
    }
  }
  return crc;
}

bool lireSHT35(float &temperature, float &humidite) {
  Wire.beginTransmission(SHT35_ADDR);
  Wire.write(0x24);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) return false;

  delay(20);

  if (Wire.requestFrom(SHT35_ADDR, (uint8_t)6) != 6) return false;
  uint8_t d[6];
  for (int i = 0; i < 6; i++) d[i] = Wire.read();

  if (crc8(d, 2) != d[2] || crc8(d + 3, 2) != d[5]) return false;

  uint16_t rawT = (d[0] << 8) | d[1];
  uint16_t rawH = (d[3] << 8) | d[4];

  temperature = -45.0 + 175.0 * rawT / 65535.0;
  humidite    = 100.0 * rawH / 65535.0;
  return true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  float t, h;
  if (lireSHT35(t, h)) {
    Serial.print("Temperature : ");
    Serial.print(t, 1);
    Serial.print(" C   Humidite : ");
    Serial.print(h, 1);
    Serial.println(" %");
  } else {
    Serial.println("Erreur de lecture du SHT35");
  }
  delay(2000);
}