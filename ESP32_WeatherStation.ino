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

  Serial.println("=== ESP32 Weather Station ===");
  #endif

  // -------------------------------------------------------------------------
  // LED Error Indicator Initialization
  // -------------------------------------------------------------------------
  errorIndicator.begin();

  // -------------------------------------------------------------------------
  // WiFi Initialization
  // -------------------------------------------------------------------------
  #if DEBUG_SERIAL_ENABLED
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
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
    Serial.println("WiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    #endif

    // Disable WiFi sleep mode for better HTTP server responsiveness
    WiFi.setSleep(false);
  } else {
    // WiFi connection failed - set error indicator
    errorIndicator.setError(ErrorType::WIFI_ERROR);

    #if DEBUG_SERIAL_ENABLED
    Serial.println("WARNING: WiFi connection failed!");
    Serial.println("System will continue without network connectivity.");
    #endif
  }

  // -------------------------------------------------------------------------
  // Sensor Initialization
  // -------------------------------------------------------------------------
  #if DEBUG_SERIAL_ENABLED
  Serial.println("Initializing I2C sensors...");
  #endif

  if (!sensorManager.begin()) {
    // Sensor initialization failed - set critical error and prepare for restart
    errorIndicator.setError(ErrorType::CRITICAL_ERROR);

    #if DEBUG_SERIAL_ENABLED
    Serial.println("CRITICAL ERROR: I2C sensor initialization failed!");
    Serial.println("Check connections: BME280 (SDA=19, SCL=21), BH1750 (SDA=27, SCL=26)");
    Serial.println("ESP32 will automatically restart in 60 seconds...");
    #endif

    // Blink LED very fast for 60 seconds, then auto-restart
    const uint32_t startTime = millis();
    const uint32_t restartDelay = 60000; // 60 seconds

    while (millis() - startTime < restartDelay) {
      errorIndicator.update();

      #if DEBUG_SERIAL_ENABLED
      // Print countdown every 10 seconds
      static uint32_t lastPrint = 0;
      if (millis() - lastPrint >= 10000) {
        lastPrint = millis();
        uint32_t remaining = (restartDelay - (millis() - startTime)) / 1000;
        Serial.printf("Auto-restart in %lu seconds...\n", remaining);
      }
      #endif

      delay(10);
    }

    #if DEBUG_SERIAL_ENABLED
    Serial.println("Restarting ESP32 now...");
    Serial.flush();
    #endif

    // Perform system restart
    ESP.restart();
  }

  #if DEBUG_SERIAL_ENABLED
  Serial.println("All sensors initialized successfully!");
  #endif

  // -------------------------------------------------------------------------
  // Web Server Initialization
  // -------------------------------------------------------------------------
  webServerManager.begin();

  #if DEBUG_SERIAL_ENABLED
  Serial.println("HTTP server started on port 80");
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
  Serial.println("\n=== System Ready ===");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Dashboard URL: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.printf("API Endpoint: http://%s/api/v1/sensors\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("Running in offline mode (sensors only)");
  }
  Serial.println("-------------------------------------------\n");
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
