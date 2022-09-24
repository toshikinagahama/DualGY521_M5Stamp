#include "Arduino.h"
#include <fastled.h>
#include <Wire.h>
#include <BluetoothSerial.h>

const int MPU_addr1 = 0x68; // I2C address of the MPU-6050
const int MPU_addr2 = 0x69; // I2C address of the MPU-6050
const int R1 = 10000;
const int R2 = 10000;
int cnt = 0;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr1);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(100);
  Wire.beginTransmission(MPU_addr2);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(26, OUTPUT);
  pinMode(36, INPUT);
  pinMode(18, INPUT);
  // digitalWrite(26, HIGH); // LEDをオン
  Serial.begin(115200);
}

void getSensorValues(const int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(address, 14, true);  // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
                                        // Serial.printf("Sensor %d: ", address);
  Serial.print("AcX = ");
  Serial.print(AcX);
  Serial.print(" | AcY = ");
  Serial.print(AcY);
  Serial.print(" | AcZ = ");
  Serial.print(AcZ);
  Serial.print(" | Tmp = ");
  Serial.print(Tmp / 340.00 + 36.53); // equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = ");
  Serial.print(GyX);
  Serial.print(" | GyY = ");
  Serial.print(GyY);
  Serial.print(" | GyZ = ");
  Serial.println(GyZ);
}

void loop(void)
{
  getSensorValues(MPU_addr1);
  getSensorValues(MPU_addr2);
  // digitalWrite(26, HIGH); // LEDをオン
  // delay(500);
  // int value = analogRead(36);
  // float vol = (float)value * (R1 + R2) / R2 * (1.85 / 4095);
  // Serial.printf("%f\n", vol);
  // digitalWrite(26, LOW); // LEDをオフ
  //    if (cnt % 1000 == 0)
  //{
  //     Serial.printf("%f\n", vol);
  //     SerialBT.printf("%f", vol);
  //     SerialBT.println("");
  //   }
  //   int value = digitalRead(18);
  //   Serial.printf("%d", value);

  // byte error, address;
  // int nDevices;

  // Serial.println("Scanning...");

  // nDevices = 0;
  // for (address = 1; address < 127; address++)
  // {
  //   // The i2c_scanner uses the return value of
  //   // the Write.endTransmisstion to see if
  //   // a device did acknowledge to the address.
  //   Wire.beginTransmission(address);
  //   error = Wire.endTransmission();

  //   if (error == 0)
  //   {
  //     Serial.print("I2C device found at address 0x");
  //     if (address < 16)
  //       Serial.print("0");
  //     Serial.print(address, HEX);
  //     Serial.println("  !");

  //     nDevices++;
  //   }
  //   else if (error == 4)
  //   {
  //     Serial.print("Unknown error at address 0x");
  //     if (address < 16)
  //       Serial.print("0");
  //     Serial.println(address, HEX);
  //   }
  // }
  // if (nDevices == 0)
  //   Serial.println("No I2C devices found\n");
  // else
  //   Serial.println("done\n");

  // delay(5000); // wait 5 seconds for next scan
  delay(100);
  cnt += 1;
}