/*
 * Web Server Manager Implementation
 */

#include "WebServerManager.h"
#include "WebContent.h"

// Constructor
WebServerManager::WebServerManager(const SensorManager& sensorManager)
  : m_server(HTTP_SERVER_PORT),
    m_sensorManager(sensorManager) {
}

// Initialize HTTP server
void WebServerManager::begin() {
  // Setup HTTP routes using lambda functions to access member methods
  m_server.on("/", [this]() { this->handleRoot(); });
  m_server.on("/api/v1/sensors", [this]() { this->handleAPI(); });
  m_server.onNotFound([this]() { this->handleNotFound(); });

  // Start the server
  m_server.begin();
}

// Handle root path - serve HTML dashboard
void WebServerManager::handleRoot() {
  m_server.send_P(200, "text/html", HTML_DASHBOARD);
}

// Handle API endpoint - serve JSON sensor data
void WebServerManager::handleAPI() {
  // Use static buffer to avoid heap fragmentation (performance optimization)
  static char jsonBuffer[JSON_BUFFER_SIZE];

  buildJSONResponse(jsonBuffer, JSON_BUFFER_SIZE);

  m_server.send(200, "application/json", jsonBuffer);
}

// Handle 404 - Not Found
void WebServerManager::handleNotFound() {
  m_server.send(404, "text/plain", "404: Not Found");
}

// Build JSON response using snprintf (faster than String concatenation)
void WebServerManager::buildJSONResponse(char* buffer, size_t bufferSize) const {
  const SensorData& data = m_sensorManager.getSensorData();

  // Calculate uptime in seconds
  const uint32_t uptimeSeconds = millis() / 1000;

  // Get WiFi signal strength (RSSI) - only if connected
  const int8_t rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : -100;

  // Build JSON dynamically based on enabled sensors
  size_t offset = 0;

  // Start JSON object
  offset += snprintf(buffer + offset, bufferSize - offset, "{");

  #if SENSOR_BME280_ENABLED
  // Add BME280 sensor data (temperature, humidity, pressure)
  if (isfinite(data.humidity)) {
    offset += snprintf(buffer + offset, bufferSize - offset,
                      "\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f",
                      data.temperature, data.humidity, data.pressure);
  } else {
    // BMP280 variant - no humidity sensor
    offset += snprintf(buffer + offset, bufferSize - offset,
                      "\"temperature\":%.2f,\"humidity\":null,\"pressure\":%.2f",
                      data.temperature, data.pressure);
  }
  #endif

  #if SENSOR_BH1750_ENABLED
  // Add comma separator if BME280 data was added
  #if SENSOR_BME280_ENABLED
  offset += snprintf(buffer + offset, bufferSize - offset, ",");
  #endif

  // Add BH1750 light sensor data
  if (isfinite(data.lightLevel)) {
    offset += snprintf(buffer + offset, bufferSize - offset,
                      "\"light\":%.2f", data.lightLevel);
  } else {
    offset += snprintf(buffer + offset, bufferSize - offset,
                      "\"light\":null");
  }
  #endif

  // Add system info (always present)
  #if SENSOR_BME280_ENABLED || SENSOR_BH1750_ENABLED
  offset += snprintf(buffer + offset, bufferSize - offset, ",");
  #endif

  offset += snprintf(buffer + offset, bufferSize - offset,
                    "\"uptime\":%lu,\"rssi\":%d,\"valid\":%s}",
                    uptimeSeconds, rssi, data.isValid ? "true" : "false");
}
