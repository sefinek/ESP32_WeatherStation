/*
 * ESP32 Weather Station - Main File
 *
 * Modular architecture with separate managers for sensors and web server
 * Optimized for performance with PROGMEM HTML and efficient JSON generation
 * Built-in LED (GPIO2) indicates system status and errors
 *
 * Hardware:
 * - ESP32 Development Board
 * - BME280/BMP280: Temperature, Humidity, Pressure (I2C Bus 1: SDA=19, SCL=21)
 * - BH1750: Light intensity sensor (I2C Bus 2: SDA=27, SCL=26)
 * - Built-in LED (GPIO2): Error/Status indicator
 */

#include <WiFi.h>
#include "Config.h"
#include "SensorManager.h"
#include "WebServerManager.h"
#include "ErrorIndicator.h"

// ============================================================================
// Global Objects
// ============================================================================
SensorManager sensorManager;
WebServerManager webServerManager(sensorManager);
ErrorIndicator errorIndicator;

// Timing
uint32_t lastMeasurementTime = 0;

// ============================================================================
// Setup Function
// ============================================================================
void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);

  #if DEBUG_SERIAL_ENABLED
  // Wait for serial connection with timeout
  while (!Serial && millis() < SERIAL_INIT_TIMEOUT_MS) {
    delay(10);
  }

  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║         ESP32 Weather Station          ║");
  Serial.println("╚════════════════════════════════════════╝");
  #endif

  // -------------------------------------------------------------------------
  // LED Error Indicator Initialization
  // -------------------------------------------------------------------------
  errorIndicator.begin();

  // -------------------------------------------------------------------------
  // WiFi Initialization
  // -------------------------------------------------------------------------
  #if DEBUG_SERIAL_ENABLED
  Serial.printf("[WiFi] Connecting to '%s'", WIFI_SSID);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection (infinite retry)
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_CONNECT_DELAY_MS);
    #if DEBUG_SERIAL_ENABLED
    Serial.print(".");
    #endif
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println();
  #endif

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    #if DEBUG_SERIAL_ENABLED
    Serial.printf("[WiFi] Connected | IP: %s | RSSI: %d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
    #endif

    // Disable WiFi sleep mode for better HTTP server responsiveness
    WiFi.setSleep(false);
  } else {
    // WiFi connection failed - set error indicator
    errorIndicator.setError(ErrorType::WIFI_ERROR);

    // Always show WiFi errors
    Serial.println("[WARN] WiFi connection failed");
    Serial.println("[WARN] System will continue in offline mode");
  }

  // -------------------------------------------------------------------------
  // Sensor Initialization
  // -------------------------------------------------------------------------
  #if DEBUG_SERIAL_ENABLED
  Serial.println("[I2C] Initializing sensors...");
  #endif

  if (!sensorManager.begin()) {
    // Sensor initialization failed - set critical error and prepare for restart
    errorIndicator.setError(ErrorType::CRITICAL_ERROR);

    // Always show critical errors
    Serial.println("\n[CRITICAL] Sensor initialization failed!");
    Serial.println("[CRITICAL] Auto-restart in 60 seconds...");

    // Blink LED very fast for 60 seconds, then auto-restart
    const uint32_t startTime = millis();
    const uint32_t restartDelay = 60000; // 60 seconds

    while (millis() - startTime < restartDelay) {
      errorIndicator.update();

      // Always print countdown for critical errors
      static uint32_t lastPrint = 0;
      if (millis() - lastPrint >= 10000) {
        lastPrint = millis();
        uint32_t remaining = (restartDelay - (millis() - startTime)) / 1000;
        Serial.printf("[CRITICAL] Restarting in %lu seconds...\n", remaining);
      }

      delay(10);
    }

    // Always show restart message for critical errors
    Serial.println("[CRITICAL] Restarting now...\n");
    Serial.flush();

    // Perform system restart
    ESP.restart();
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println("[I2C] All sensors ready");
  #endif

  // -------------------------------------------------------------------------
  // Web Server Initialization
  // -------------------------------------------------------------------------
  webServerManager.begin();

  #if DEBUG_SERIAL_ENABLED
  Serial.printf("[HTTP] Server started on port %d\n", HTTP_SERVER_PORT);
  #endif

  // -------------------------------------------------------------------------
  // System Ready - Final Error State Check
  // -------------------------------------------------------------------------
  // If we reached here, sensors initialized successfully
  // Now determine final system state based on WiFi status
  if (WiFi.status() != WL_CONNECTED) {
    // Sensors OK but WiFi failed - set WiFi error
    errorIndicator.setError(ErrorType::WIFI_ERROR);
  } else {
    // All systems operational - clear error state
    errorIndicator.setError(ErrorType::NONE);
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║             SYSTEM READY               ║");
  Serial.println("╚════════════════════════════════════════╝");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("→ Dashboard: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.printf("→ API:       http://%s/api/v1/sensors\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("→ Mode: Offline (sensors only)");
  }
  Serial.println();
  #endif
}

// ============================================================================
// Main Loop
// ============================================================================
void loop() {
  // Update LED error indicator based on current error state
  errorIndicator.update();

  // Handle incoming HTTP client requests
  webServerManager.handleClient();

  const uint32_t currentTime = millis();

  // Periodic sensor readings based on configured interval (5 seconds)
  if (currentTime - lastMeasurementTime >= MEASUREMENT_INTERVAL_MS) {
    lastMeasurementTime = currentTime;

    // Trigger sensor reading (BME280 + BH1750)
    sensorManager.readSensors();

    // Monitor sensor health and update error status accordingly
    const SensorData& data = sensorManager.getSensorData();

    if (!data.isValid && errorIndicator.getCurrentError() == ErrorType::NONE) {
      // Sensors were working but now returning invalid data
      errorIndicator.setError(ErrorType::SENSOR_ERROR);
    } else if (data.isValid && errorIndicator.getCurrentError() == ErrorType::SENSOR_ERROR) {
      // Sensors recovered from error state - update status
      if (WiFi.status() == WL_CONNECTED) {
        // Both sensors and WiFi are OK
        errorIndicator.setError(ErrorType::NONE);
      } else {
        // Sensors OK but WiFi still down
        errorIndicator.setError(ErrorType::WIFI_ERROR);
      }
    }

    // Output sensor readings to serial monitor (debug mode only)
    sensorManager.printToSerial();
  }
}
