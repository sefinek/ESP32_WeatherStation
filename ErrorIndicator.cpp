/*
 * Error Indicator Implementation
 */

#include "ErrorIndicator.h"

// Constructor
ErrorIndicator::ErrorIndicator()
  : m_currentError(ErrorType::NONE),
    m_ledState(false),
    m_lastToggleTime(0) {
}

// Initialize LED pin
void ErrorIndicator::begin() {
  pinMode(LED_PIN, OUTPUT);
  setLED(false);
}

// Set current error state
void ErrorIndicator::setError(ErrorType error) {
  if (m_currentError != error) {
    m_currentError = error;
    m_lastToggleTime = millis();
    setLED(false);

    #if DEBUG_SERIAL_ENABLED
    // Only log errors, not normal status
    if (error != ErrorType::NONE) {
      switch (error) {
        case ErrorType::WIFI_ERROR:
          Serial.println("ERROR: WiFi failed");
          break;
        case ErrorType::SENSOR_ERROR:
          Serial.println("ERROR: Sensor malfunction");
          break;
        case ErrorType::CRITICAL_ERROR:
          Serial.println("CRITICAL: I2C sensor init failed");
          break;
        default:
          break;
      }
    }
    #endif
  }
}

// Update LED state based on current error
void ErrorIndicator::update() {
  const uint32_t currentTime = millis();

  switch (m_currentError) {
    case ErrorType::NONE: {
      // Normal operation: LED off (energy saving)
      // No action needed, LED stays off
      break;
    }

    case ErrorType::WIFI_ERROR:
    case ErrorType::SENSOR_ERROR:
    case ErrorType::CRITICAL_ERROR: {
      // Error blinking patterns
      const uint16_t duration = m_ledState ? getOnDuration() : getOffDuration();

      if (currentTime - m_lastToggleTime >= duration) {
        m_lastToggleTime = currentTime;
        setLED(!m_ledState);
      }
      break;
    }
  }
}

// Get ON duration for current error
uint16_t ErrorIndicator::getOnDuration() const {
  switch (m_currentError) {
    case ErrorType::WIFI_ERROR:
      return LED_BLINK_WIFI_ERROR_ON;
    case ErrorType::SENSOR_ERROR:
      return LED_BLINK_SENSOR_ERROR_ON;
    case ErrorType::CRITICAL_ERROR:
      return LED_BLINK_CRITICAL_ERROR_ON;
    default:
      return 0;
  }
}

// Get OFF duration for current error
uint16_t ErrorIndicator::getOffDuration() const {
  switch (m_currentError) {
    case ErrorType::WIFI_ERROR:
      return LED_BLINK_WIFI_ERROR_OFF;
    case ErrorType::SENSOR_ERROR:
      return LED_BLINK_SENSOR_ERROR_OFF;
    case ErrorType::CRITICAL_ERROR:
      return LED_BLINK_CRITICAL_ERROR_OFF;
    default:
      return 0;
  }
}
