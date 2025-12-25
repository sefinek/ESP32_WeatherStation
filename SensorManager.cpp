/*
 * Sensor Manager Implementation
 */

#include "SensorManager.h"

// Constructor - initialize sensor objects
SensorManager::SensorManager()
  : m_lightMeter(BH1750_I2C_ADDR) {
}

// Initialize all sensors
bool SensorManager::begin() {
  bool success = true;

  // Initialize BME280/BMP280
  if (!initBME280()) {
    success = false;
  }

  // Initialize BH1750
  if (!initBH1750()) {
    success = false;
  }

  return success;
}

// Initialize BME280/BMP280 on I2C Bus #1
bool SensorManager::initBME280() {
  // Initialize I2C Bus #1 with custom pins
  Wire.begin(I2C1_SDA_PIN, I2C1_SCL_PIN);
  Wire.setClock(I2C_CLOCK_SPEED);

  #if DEBUG_SERIAL_ENABLED
  Serial.printf("I2C Bus #1: SDA=%d, SCL=%d @ %dHz\n", I2C1_SDA_PIN, I2C1_SCL_PIN, I2C_CLOCK_SPEED);
  #endif

  // Try to initialize BME280/BMP280 sensor
  if (!m_bme.begin(BME_I2C_ADDR, &Wire)) {
    #if DEBUG_SERIAL_ENABLED
    Serial.println("ERROR: BME280/BMP280 not found at 0x76");
    #endif
    return false;
  }

  // Configure sensor for fast measurements (all x2 oversampling)
  m_bme.setSampling(
    Adafruit_BME280::MODE_FORCED,        // Forced mode (power saving)
    Adafruit_BME280::SAMPLING_X2,        // Temp x2: ±0.5°C, fast
    Adafruit_BME280::SAMPLING_X2,        // Humidity x2: ±0.5%, fast
    Adafruit_BME280::SAMPLING_X2,        // Pressure x2: ±2 Pa, fast
    Adafruit_BME280::FILTER_OFF,         // No IIR filter (instant response)
    Adafruit_BME280::STANDBY_MS_1000     // Standby (unused in FORCED mode)
  );

  #if DEBUG_SERIAL_ENABLED
  Serial.println("BME280/BMP280 initialized OK");
  #endif

  return true;
}

// Initialize BH1750 on I2C Bus #2
bool SensorManager::initBH1750() {
  // Initialize I2C Bus #2 with custom pins (separate bus for isolation)
  Wire1.begin(I2C2_SDA_PIN, I2C2_SCL_PIN);
  Wire1.setClock(I2C_CLOCK_SPEED);

  #if DEBUG_SERIAL_ENABLED
  Serial.printf("I2C Bus #2: SDA=%d, SCL=%d @ %dHz\n", I2C2_SDA_PIN, I2C2_SCL_PIN, I2C_CLOCK_SPEED);
  #endif

  // Try to initialize BH1750 light sensor
  if (!m_lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_I2C_ADDR, &Wire1)) {
    #if DEBUG_SERIAL_ENABLED
    Serial.println("ERROR: BH1750 light sensor not found at 0x23");
    #endif
    return false;
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println("BH1750 light sensor initialized OK");
  #endif

  return true;
}

// Read all sensors and update internal data
void SensorManager::readSensors() {
  // Trigger forced measurement on BME280 (wakes sensor from sleep)
  m_bme.takeForcedMeasurement();

  m_sensorData.temperature = m_bme.readTemperature();
  m_sensorData.humidity = m_bme.readHumidity();
  m_sensorData.pressure = m_bme.readPressure();
  m_sensorData.lightLevel = m_lightMeter.readLightLevel();

  // Validate all readings
  validateReadings();
}

// Validate sensor readings
void SensorManager::validateReadings() {
  // Check if critical sensor readings are valid (finite numbers)
  // Temperature, pressure, and light must all be valid
  m_sensorData.isValid = isfinite(m_sensorData.temperature) &&
                         isfinite(m_sensorData.pressure) &&
                         isfinite(m_sensorData.lightLevel);

  // Note: Humidity might be NaN on BMP280 (lacks humidity sensor)
  // This is acceptable and doesn't affect overall validity
}

// Print sensor readings to Serial
void SensorManager::printToSerial() const {
  #if DEBUG_SERIAL_ENABLED
  // Only log errors to reduce serial output spam
  // Valid readings are accessible via web dashboard
  if (!m_sensorData.isValid) {
    Serial.println("WARNING: Invalid sensor data detected");
  }
  #endif
}
