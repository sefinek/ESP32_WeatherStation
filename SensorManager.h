/*
 * Sensor Manager for ESP32 Weather Station
 * Handles BME280/BMP280 and BH1750 sensors on separate I2C buses
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include "Config.h"

class SensorManager {
public:
  // Constructor
  SensorManager();

  // Initialize both sensors and I2C buses
  // Returns true if all sensors initialized successfully
  bool begin();

  // Read all sensors and update internal data
  void readSensors();

  // Get current sensor readings (const reference to avoid copying)
  inline const SensorData& getSensorData() const {
    return m_sensorData;
  }

  // Print sensor readings to Serial (only if DEBUG_SERIAL_ENABLED)
  void printToSerial() const;

private:
  // Sensor objects
  Adafruit_BME280 m_bme;
  BH1750 m_lightMeter;

  // Current sensor readings
  SensorData m_sensorData;

  // Internal initialization methods
  bool initBME280();
  bool initBH1750();

  // Validate sensor readings
  void validateReadings();
};

#endif // SENSOR_MANAGER_H
