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
  // Send HTML from PROGMEM (Flash memory)
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

  // Build JSON using snprintf for better performance and no heap fragmentation
  if (isfinite(data.humidity)) {
    snprintf(buffer, bufferSize,
             "{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"light\":%.2f,\"uptime\":%lu,\"rssi\":%d,\"valid\":%s}",
             data.temperature,
             data.humidity,
             data.pressure,
             data.lightLevel,
             uptimeSeconds,
             rssi,
             data.isValid ? "true" : "false");
  } else {
    // Handle case where humidity is not available (BMP280)
    snprintf(buffer, bufferSize,
             "{\"temperature\":%.2f,\"humidity\":null,\"pressure\":%.2f,\"light\":%.2f,\"uptime\":%lu,\"rssi\":%d,\"valid\":%s}",
             data.temperature,
             data.pressure,
             data.lightLevel,
             uptimeSeconds,
             rssi,
             data.isValid ? "true" : "false");
  }
}
