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

  #if SENSOR_BME280_ENABLED
  // Initialize BME280/BMP280
  if (!initBME280()) {
    success = false;
  }
  #else
  #if DEBUG_SERIAL_ENABLED
  Serial.println("[I2C] BME280 sensor disabled");
  #endif
  #endif

  #if SENSOR_BH1750_ENABLED
  // Initialize BH1750
  if (!initBH1750()) {
    success = false;
  }
  #else
  #if DEBUG_SERIAL_ENABLED
  Serial.println("[I2C] BH1750 sensor disabled");
  #endif
  #endif

  return success;
}

// Initialize BME280/BMP280 on I2C Bus #1
bool SensorManager::initBME280() {
  // Initialize I2C Bus #1 with custom pins
  Wire.begin(I2C1_SDA_PIN, I2C1_SCL_PIN);
  Wire.setClock(I2C_CLOCK_SPEED);

  #if DEBUG_SERIAL_ENABLED
  Serial.printf("[I2C] Bus #1 init: SDA=%d, SCL=%d @ %dkHz\n",
                I2C1_SDA_PIN, I2C1_SCL_PIN, I2C_CLOCK_SPEED / 1000);
  #endif

  // Try to initialize BME280/BMP280 sensor
  if (!m_bme.begin(BME_I2C_ADDR, &Wire)) {
    Serial.println("[ERROR] BME280 not found at 0x76");
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
  Serial.println("[I2C] BME280 ready");
  #endif

  return true;
}

// Initialize BH1750 on I2C Bus #2
bool SensorManager::initBH1750() {
  // Initialize I2C Bus #2 with custom pins (separate bus for isolation)
  Wire1.begin(I2C2_SDA_PIN, I2C2_SCL_PIN);
  Wire1.setClock(I2C_CLOCK_SPEED);

  #if DEBUG_SERIAL_ENABLED
  Serial.printf("[I2C] Bus #2 init: SDA=%d, SCL=%d @ %dkHz\n",
                I2C2_SDA_PIN, I2C2_SCL_PIN, I2C_CLOCK_SPEED / 1000);
  #endif

  // Try to initialize BH1750 light sensor
  if (!m_lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_I2C_ADDR, &Wire1)) {
    // Always show sensor errors
    Serial.println("[ERROR] BH1750 not found at 0x23");
    return false;
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println("[I2C] BH1750 ready");
  #endif

  return true;
}

// Read all sensors and update internal data
void SensorManager::readSensors() {
  #if SENSOR_BME280_ENABLED
  // Trigger forced measurement on BME280 (wakes sensor from sleep)
  m_bme.takeForcedMeasurement();

  m_sensorData.temperature = m_bme.readTemperature();
  m_sensorData.humidity = m_bme.readHumidity();
  m_sensorData.pressure = m_bme.readPressure();
  #else
  m_sensorData.temperature = NAN;
  m_sensorData.humidity = NAN;
  m_sensorData.pressure = NAN;
  #endif

  #if SENSOR_BH1750_ENABLED
  m_sensorData.lightLevel = m_lightMeter.readLightLevel();
  #else
  m_sensorData.lightLevel = NAN;
  #endif

  // Validate all readings
  validateReadings();
}

// Validate sensor readings
void SensorManager::validateReadings() {
  // Start with valid assumption
  m_sensorData.isValid = true;

  #if SENSOR_BME280_ENABLED
  // Check if BME280 readings are valid (temperature and pressure are critical)
  if (!isfinite(m_sensorData.temperature) || !isfinite(m_sensorData.pressure)) {
    m_sensorData.isValid = false;
  }
  // Note: Humidity might be NaN on BMP280 (lacks humidity sensor)
  // This is acceptable and doesn't affect overall validity
  #endif

  #if SENSOR_BH1750_ENABLED
  // Check if BH1750 reading is valid
  if (!isfinite(m_sensorData.lightLevel)) {
    m_sensorData.isValid = false;
  }
  #endif
}

// Print sensor readings to Serial
void SensorManager::printToSerial() const {
  #if DEBUG_SERIAL_ENABLED
  // Only log errors to reduce serial output spam
  // Valid readings are accessible via web dashboard
  if (!m_sensorData.isValid) {
    Serial.println("[WARN] Invalid sensor data");
  }
  #endif
}
