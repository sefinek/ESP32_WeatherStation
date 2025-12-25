/*
 * Web Server Manager for ESP32 Weather Station
 * Handles HTTP server and routes
 */

#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include "Config.h"
#include "SensorManager.h"

class WebServerManager {
public:
  // Constructor - takes reference to SensorManager for data access
  WebServerManager(const SensorManager& sensorManager);

  // Initialize and start HTTP server
  void begin();

  // Handle incoming HTTP requests (call in loop)
  inline void handleClient() {
    m_server.handleClient();
  }

private:
  // HTTP server object
  WebServer m_server;

  // Reference to sensor manager for reading data
  const SensorManager& m_sensorManager;

  // HTTP route handlers
  void handleRoot();
  void handleAPI();
  void handleNotFound();

  // Helper method to build JSON response (optimized with static buffer)
  void buildJSONResponse(char* buffer, size_t bufferSize) const;
};

#endif // WEB_SERVER_MANAGER_H
