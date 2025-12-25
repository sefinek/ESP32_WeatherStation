/*
 * Configuration file for ESP32 Weather Station
 * Contains all hardware pins, WiFi credentials, and system parameters
 *
 * INSTRUCTIONS:
 * 1. Copy this file and rename it to "Config.h"
 * 2. Replace the placeholder values with your actual WiFi credentials
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// WiFi Configuration
// ============================================================================
constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";
constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
constexpr uint8_t WIFI_MAX_CONNECT_ATTEMPTS = 30;
constexpr uint16_t WIFI_CONNECT_DELAY_MS = 7000;

// ============================================================================
// I2C Bus #1 - BME280/BMP280 Configuration
// ============================================================================
constexpr uint8_t I2C1_SDA_PIN = 19;
constexpr uint8_t I2C1_SCL_PIN = 21;
constexpr uint8_t BME_I2C_ADDR = 0x76;

// ============================================================================
// I2C Bus #2 - BH1750 Configuration
// ============================================================================
constexpr uint8_t I2C2_SDA_PIN = 27;
constexpr uint8_t I2C2_SCL_PIN = 26;
constexpr uint8_t BH1750_I2C_ADDR = 0x23;

// ============================================================================
// I2C Common Configuration
// ============================================================================
constexpr uint32_t I2C_CLOCK_SPEED = 100000;  // 100 kHz (Standard Mode, power saving)

// ============================================================================
// Measurement Configuration
// ============================================================================
constexpr uint32_t MEASUREMENT_INTERVAL_MS = 5000;  // 5 seconds between readings

// ============================================================================
// Serial Communication
// ============================================================================
constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr uint16_t SERIAL_INIT_TIMEOUT_MS = 2000;

// ============================================================================
// HTTP Server Configuration
// ============================================================================
constexpr uint16_t HTTP_SERVER_PORT = 80;
constexpr uint16_t JSON_BUFFER_SIZE = 200;

// ============================================================================
// Debug Configuration
// ============================================================================
#define DEBUG_SERIAL_ENABLED false  // Set to false to disable serial output for performance

// ============================================================================
// LED Error Indicator Configuration
// ============================================================================
constexpr uint8_t LED_PIN = 2;  // Built-in LED on ESP32

// LED blink patterns (ON time, OFF time in milliseconds)
constexpr uint16_t LED_BLINK_WIFI_ERROR_ON = 100;   // Fast blink for WiFi error
constexpr uint16_t LED_BLINK_WIFI_ERROR_OFF = 100;
constexpr uint16_t LED_BLINK_SENSOR_ERROR_ON = 300; // Medium blink for sensor error
constexpr uint16_t LED_BLINK_SENSOR_ERROR_OFF = 300;
constexpr uint16_t LED_BLINK_CRITICAL_ERROR_ON = 50; // Very fast blink for critical ESP error
constexpr uint16_t LED_BLINK_CRITICAL_ERROR_OFF = 50;

// ============================================================================
// Error Types Enumeration
// ============================================================================
enum class ErrorType : uint8_t {
  NONE = 0,           // No error - system OK
  WIFI_ERROR = 1,     // WiFi connection failed
  SENSOR_ERROR = 2,   // Sensor initialization/reading failed
  CRITICAL_ERROR = 3  // Critical system error
};

// ============================================================================
// Sensor Data Structure
// ============================================================================
struct SensorData {
  float temperature = 0.0f;
  float humidity = 0.0f;
  float pressure = 0.0f;
  float lightLevel = 0.0f;
  bool isValid = false;

  // Constructor
  SensorData() = default;

  // Copy constructor
  SensorData(const SensorData&) = default;

  // Assignment operator
  SensorData& operator=(const SensorData&) = default;
};

#endif // CONFIG_H
