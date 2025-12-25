# ESP-WROOM-32 Outdoor Weather Station

Advanced weather monitoring system for ESP-WROOM-32 Ideaspark with real-time web dashboard and API.

> **Note:** This README was generated with AI assistance.

## Features

- Real-time sensor monitoring (temperature, humidity, pressure, light)
- Modern web dashboard with live updates every 5 seconds
- RESTful JSON API
- Advanced meteorological calculations (dew point, feels like, comfort level)
- Dynamic color-coded values based on sensor readings
- WiFi signal strength monitoring
- System uptime tracking
- Moon phase calculation
- Pressure trend analysis with 3-hour history

## Hardware

- **ESP32** Development Board
- **BME280/BMP280** - Temperature, Humidity, Pressure (I2C Bus 1)
  - SDA: GPIO 19
  - SCL: GPIO 21
  - Address: 0x76
- **BH1750** - Digital Light Sensor (I2C Bus 2)
  - SDA: GPIO 27
  - SCL: GPIO 26
  - Address: 0x23
- **Built-in LED** (GPIO 2) - Error/Status indicator

## Software Architecture

```
ESP32_WeatherStation.ino  - Main application
Config.h                  - Configuration & constants
SensorManager.h/cpp       - Sensor handling & validation
WebServerManager.h/cpp    - HTTP server & API
WebContent.h              - HTML dashboard (PROGMEM)
ErrorIndicator.h/cpp      - LED error indication
```

## API Endpoints

### GET /
Web dashboard with real-time sensor visualization

### GET /api/v1/sensors
JSON sensor data
```json
{
  "temperature": 24.18,
  "humidity": 58.39,
  "pressure": 102256,
  "light": 20.00,
  "uptime": 92,
  "rssi": -62,
  "valid": true
}
```

## Configuration

Edit `Config.h` to customize:
- WiFi credentials (SSID/Password)
- I2C pins and addresses
- Measurement interval (default: 5s)
- Serial debug output
- LED error patterns

## Dependencies

- Adafruit BME280 Library
- BH1750 Library
- ESP32 Arduino Core

## Performance Optimizations

- HTML stored in PROGMEM (Flash) - saves ~5KB RAM
- Static JSON buffer - no heap fragmentation
- FORCED mode on BME280 - power saving
- 100kHz I2C clock - energy efficient
- Moon phase caching - calculated once per day
- No IIR filtering - instant temperature response

## Installation

1. Install required libraries via Arduino Library Manager
2. Update WiFi credentials in `Config.h`
3. Upload to ESP32
4. Connect to `http://<ESP32-IP>` in browser

## Error Indication (GPIO 2 LED)

- **OFF** - System OK
- **Fast blink (100ms)** - WiFi connection failed
- **Medium blink (300ms)** - Sensor error
- **Very fast blink (50ms)** - Critical error (auto-restart in 60s)

## License

Copyright (c) 2025 Sefinek. Licensed under MIT.