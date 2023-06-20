#include <Adafruit_GPS.h>
#include <Adafruit_MPU6050.h>
#include <Arduino.h>
#include <due_can.h>

#ifndef MONITOR_SPEED
#define MONITOR_SPEED 115200
#endif

Adafruit_GPS gps(&Serial3);
Adafruit_MPU6050 mpu;

bool gps_status = false, mpu_status = false;

char c;

void setup() {
  // CAN
  Can0.init(CAN_BPS_500K);
  // GPS
  Serial.begin(MONITOR_SPEED);
  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  mpu.begin();
}

void clearGPS() {
  while (!gps.newNMEAreceived()) {
    c = gps.read();
  }
  gps.parse(gps.lastNMEA());

  while (!gps.newNMEAreceived()) {
    c = gps.read();
  }
  gps.parse(gps.lastNMEA());
}

void loop() {
  // GPS
  clearGPS();

  while (!gps.newNMEAreceived()) {
    c = gps.read();
  }

  gps.parse(gps.lastNMEA());

  Serial.print("Time: ");
  Serial.print(gps.hour, DEC);
  Serial.print(':');
  Serial.print(gps.minute, DEC);
  Serial.print(':');
  Serial.print(gps.seconds, DEC);
  Serial.print('.');
  Serial.print(gps.milliseconds);

  Serial.print(" | Date: ");
  Serial.print(gps.day, DEC);
  Serial.print('/');
  Serial.print(gps.month, DEC);
  Serial.print("/20");
  Serial.print(gps.year, DEC);

  Serial.print(" | Fix: ");
  Serial.print(gps.fix);
  Serial.print(" quality: ");
  Serial.print(gps.fixquality);
  Serial.print(" | Satellites: ");
  Serial.println(gps.satellites);

  if (gps.fix) {
    Serial.print("Location: ");
    Serial.print(gps.latitude, 4);
    Serial.print(gps.lat);
    Serial.print(", ");
    Serial.print(gps.longitude, 4);
    Serial.print(gps.lon);
    Serial.print(" | Google Maps location: ");
    Serial.print(gps.latitudeDegrees, 4);
    Serial.print(", ");
    Serial.print(gps.longitudeDegrees, 4);

    Serial.print(" | Speed (knots): ");
    Serial.print(gps.speed);
    Serial.print(" | Heading: ");
    Serial.print(gps.angle);
    Serial.print(" | Altitude: ");
    Serial.println(gps.altitude);
  }

  // Accel
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);
  {
    CAN_FRAME can_frame;
    can_frame.id = 0x21;
    can_frame.length = 8;
    can_frame.data.uint32[0] = *(uint32_t *)&accel.acceleration.x;
    can_frame.data.uint32[1] = *(uint32_t *)&accel.acceleration.y;
    Can0.sendFrame(can_frame);
  }
  {
    CAN_FRAME can_frame;
    can_frame.id = 0x22;
    can_frame.length = 8;
    can_frame.data.uint32[0] = *(uint32_t *)&accel.acceleration.z;
    can_frame.data.uint32[1] = *(uint32_t *)&gyro.gyro.x;
    Can0.sendFrame(can_frame);
  }
  {
    CAN_FRAME can_frame;
    can_frame.id = 0x23;
    can_frame.length = 8;
    can_frame.data.uint32[0] = *(uint32_t *)&gyro.gyro.y;
    can_frame.data.uint32[1] = *(uint32_t *)&gyro.gyro.z;
    Can0.sendFrame(can_frame);
  }
  {
    CAN_FRAME can_frame;
    can_frame.id = 0x24;
    can_frame.length = 4;
    can_frame.data.uint32[0] = *(uint32_t *)&temp.temperature;
    Can0.sendFrame(can_frame);
  }

  Serial.print("aX = ");
  Serial.print(accel.acceleration.x);
  Serial.print(" | aY = ");
  Serial.print(accel.acceleration.y);
  Serial.print(" | aZ = ");
  Serial.print(accel.acceleration.z);
  Serial.print(" | tmp = ");
  Serial.print(temp.temperature);
  Serial.print(" | gX = ");
  Serial.print(gyro.gyro.x);
  Serial.print(" | gY = ");
  Serial.print(gyro.gyro.y);
  Serial.print(" | gZ = ");
  Serial.println(gyro.gyro.z);
}
