# ESP-WROOM-32 Outdoor Weather Station
Advanced weather monitoring system for ESP-WROOM-32 Ideaspark with real-time web dashboard and API.

> **Note:** This README was generated with AI assistance.

## Features
- Real-time sensor monitoring (temperature, humidity, pressure, light)
- **Optional sensor architecture** - Enable/disable sensors via compile-time switches
- Modern web dashboard with live updates every 5 seconds
- RESTful JSON API with conditional sensor data
- Advanced meteorological calculations (dew point, feels like, comfort level)
- Dynamic color-coded values based on sensor readings
- Graceful degradation - displays 'N/A' for disabled sensors
- Wi-Fi signal strength monitoring
- System uptime tracking
- Moon phase calculation
- Pressure trend analysis with 3-hour history

## Hardware
- **ESP32** Development Board
- **BME280/BMP280** - Temperature, Humidity, Pressure (I2C Bus 1) *[Optional]*
  - SDA: GPIO 19
  - SCL: GPIO 21
  - Address: 0x76
  - Enable with: `SENSOR_BME280_ENABLED true`
- **BH1750** - Digital Light Sensor (I2C Bus 2) *[Optional]*
  - SDA: GPIO 27
  - SCL: GPIO 26
  - Address: 0x23
  - Enable with: `SENSOR_BH1750_ENABLED true`
- **Built-in LED** (GPIO 2) - Error/Status indicator

> **Note:** At least one sensor (BME280 or BH1750) must be enabled for the system to function.

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
JSON sensor data with conditional fields based on enabled sensors

**Example with all sensors enabled:**
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

**Example with only BME280 enabled (BH1750 disabled):**
```json
{
  "temperature": 24.18,
  "humidity": 58.39,
  "pressure": 102256,
  "uptime": 92,
  "rssi": -62,
  "valid": true
}
```

> **Note:** The API only includes fields for enabled sensors. Disabled sensors are omitted from the JSON response. The `humidity` field may be `null` if the BME280 sensor fails to read humidity (e.g., when using BMP280).

## Configuration
Edit `Config.h` to customize:
- **Sensor Enable/Disable Switches** (NEW)
  - `SENSOR_BME280_ENABLED` - Enable/disable BME280/BMP280 sensor
  - `SENSOR_BH1750_ENABLED` - Enable/disable BH1750 light sensor
- WiFi credentials (SSID/Password)
- I2C pins and addresses
- Measurement interval (default: 5s)
- Serial debug output
- LED error patterns

### Sensor Configuration Examples
```cpp
// Both sensors enabled (default)
#define SENSOR_BME280_ENABLED true
#define SENSOR_BH1750_ENABLED true

// Only BME280 enabled
#define SENSOR_BME280_ENABLED true
#define SENSOR_BH1750_ENABLED false

// Only BH1750 enabled
#define SENSOR_BME280_ENABLED false
#define SENSOR_BH1750_ENABLED true
```

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

## System Integration
This weather station integrates with two external projects for data persistence and advanced visualization:

### Backend (sefinek-cron)
- Collects sensor data from the ESP32 API endpoint
- Stores data in MongoDB with optional sensor fields
- Validates that at least one sensor (BME280 or BH1750) is active
- Aggregates data with null filtering for disabled sensors
- **Path:** `D:\Projects\other\sefinek-cron`

### Frontend (sefinek.net)
- Modern web dashboard with Chart.js visualization
- Displays real-time and historical sensor data
- Gracefully handles missing sensors by showing 'N/A'
- Smooth gradient animations on dashboard
- Chart gap rendering when sensors are disabled (`spanGaps: false`)
- **Path:** `D:\Projects\www\sefinek.net`

> **Note:** Both projects automatically adapt to the enabled/disabled sensor configuration. No manual changes required when toggling sensors.

## Installation
1. Install required libraries via Arduino Library Manager
   - Adafruit BME280 Library
   - BH1750 Library
2. Configure sensors in `Config.h`
   - Set `SENSOR_BME280_ENABLED` and `SENSOR_BH1750_ENABLED` based on your hardware
   - Update WiFi credentials (SSID/Password)
3. Upload to ESP32
4. Connect to `http://<ESP32-IP>` in browser
5. Verify sensor data is displayed (or 'N/A' for disabled sensors)

## Error Indication (GPIO 2 LED)
- **OFF** - System OK
- **Fast blink (100ms)** - WiFi connection failed
- **Medium blink (300ms)** - Sensor error
- **Very fast blink (50ms)** - Critical error (auto-restart in 60s)

## Technical Implementation

### Optional Sensor Architecture
The system uses compile-time switches to enable/disable sensors:
- **Firmware Level:** `#if SENSOR_BME280_ENABLED` conditionally compiles sensor code
- **API Level:** JSON response dynamically includes only enabled sensor fields
- **Frontend Level:** JavaScript checks for `null`/`undefined` values and displays 'N/A'

### Data Flow
```
ESP32 (sensors) → JSON API → sefinek-cron (MongoDB) → sefinek.net (visualization)
```

### Null Handling Strategy
- **ESP32:** Disabled sensors return `NAN` values, excluded from JSON
- **MongoDB:** Schema has optional fields (`required: false`)
- **Backend:** Aggregation filters null/undefined/NaN before averaging
- **Frontend:** Chart.js uses `spanGaps: false` to show gaps, text displays show 'N/A'

### Key Files Modified
- `Config.h` - Sensor enable/disable switches (lines 21-22)
- `WebServerManager.cpp` - Dynamic JSON builder (lines 46-100)
- `WebContent.h` - Null-safe value display (lines 553-625)
- `SensorManager.cpp` - Conditional sensor reading (lines 102-145)

## License
Copyright (c) 2025 Sefinek. Licensed under MIT.
