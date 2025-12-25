/*
 * Error Indicator Manager for ESP32 Weather Station
 * Controls built-in LED to signal different system states
 */

#ifndef ERROR_INDICATOR_H
#define ERROR_INDICATOR_H

#include <Arduino.h>
#include "Config.h"

class ErrorIndicator {
public:
  // Constructor
  ErrorIndicator();

  // Initialize LED pin
  void begin();

  // Set current error state
  void setError(ErrorType error);

  // Update LED state (call in loop)
  void update();

  // Get current error type
  inline ErrorType getCurrentError() const {
    return m_currentError;
  }

private:
  // Current error state
  ErrorType m_currentError;

  // LED state tracking
  bool m_ledState;
  uint32_t m_lastToggleTime;

  // Get ON duration for current error
  uint16_t getOnDuration() const;

  // Get OFF duration for current error
  uint16_t getOffDuration() const;

  // Set LED state
  inline void setLED(bool state) {
    digitalWrite(LED_PIN, state ? HIGH : LOW);
    m_ledState = state;
  }
};

#endif // ERROR_INDICATOR_H
