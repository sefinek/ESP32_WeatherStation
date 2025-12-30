#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H

#include <Arduino.h>

const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Weather Station</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
      min-height: 100vh;
      padding: 20px;
      display: flex;
      justify-content: center;
      align-items: center;
    }
    .container {
      background: rgba(26, 26, 46, 0.95);
      border: 1px solid rgba(255, 255, 255, 0.1);
      padding: 35px;
      border-radius: 25px;
      box-shadow: 0 25px 80px rgba(0, 0, 0, 0.6);
      max-width: 1300px;
      width: 100%;
    }
    h1 {
      color: #00d4ff;
      margin-bottom: 10px;
      font-size: 34px;
      text-align: center;
      text-shadow: 0 0 25px rgba(0, 212, 255, 0.6);
      letter-spacing: 1.2px;
      font-weight: 700;
    }
    .subtitle {
      text-align: center;
      color: #64ffda;
      margin-bottom: 28px;
      font-size: 14px;
      letter-spacing: 1px;
      opacity: 0.9;
      font-weight: 500;
    }
    .status {
      text-align: center;
      padding: 14px 20px;
      margin-bottom: 28px;
      border-radius: 14px;
      background: linear-gradient(135deg, #10b981 0%, #059669 100%);
      color: #0f1419;
      font-weight: 700;
      font-size: 15px;
      box-shadow: 0 4px 20px rgba(16, 185, 129, 0.4);
      transition: all 0.3s ease;
      letter-spacing: 0.5px;
    }
    .status.error {
      background: linear-gradient(135deg, #ff416c 0%, #ff4b2b 100%);
      color: white;
      box-shadow: 0 4px 20px rgba(255, 65, 108, 0.4);
    }
    .sensor-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
      gap: 18px;
      margin-bottom: 25px;
    }
    .sensor-card {
      background: linear-gradient(135deg, rgba(0, 212, 255, 0.15) 0%, rgba(15, 52, 96, 0.4) 100%);
      border: 1px solid rgba(0, 212, 255, 0.3);
      color: #e6f1ff;
      padding: 22px 18px;
      border-radius: 15px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
      transition: all 0.3s ease;
      text-align: center;
      display: flex;
      flex-direction: column;
      justify-content: center;
      min-height: 120px;
    }
    .sensor-card:hover {
      transform: translateY(-5px);
      box-shadow: 0 8px 30px rgba(0, 212, 255, 0.4);
      border-color: rgba(0, 212, 255, 0.6);
    }
    .sensor-label {
      font-size: 12px;
      color: #8892b0;
      margin-bottom: 12px;
      text-transform: uppercase;
      letter-spacing: 1.2px;
      font-weight: 600;
    }
    .sensor-value {
      font-size: 36px;
      font-weight: bold;
      color: #00d4ff;
      text-shadow: 0 0 12px rgba(0, 212, 255, 0.6);
      line-height: 1;
      transition: all 0.3s ease;
    }
    .sensor-unit {
      font-size: 20px;
      color: #64ffda;
      opacity: 0.85;
      margin-left: 6px;
      font-weight: 500;
    }
    .info-section {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      gap: 15px;
      margin-bottom: 25px;
    }
    .info-card {
      background: linear-gradient(135deg, rgba(100, 255, 218, 0.12) 0%, rgba(15, 52, 96, 0.4) 100%);
      border: 1px solid rgba(100, 255, 218, 0.25);
      color: #e6f1ff;
      padding: 20px 16px;
      border-radius: 15px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
      transition: all 0.3s ease;
      text-align: center;
      display: flex;
      flex-direction: column;
      justify-content: center;
      min-height: 100px;
    }
    .info-card:hover {
      transform: translateY(-3px);
      box-shadow: 0 6px 25px rgba(100, 255, 218, 0.3);
      border-color: rgba(100, 255, 218, 0.5);
    }
    .info-label {
      font-size: 11px;
      color: #8892b0;
      margin-bottom: 10px;
      text-transform: uppercase;
      letter-spacing: 1.2px;
      font-weight: 600;
    }
    .info-value {
      font-size: 22px;
      font-weight: bold;
      color: #64ffda;
      text-shadow: 0 0 10px rgba(100, 255, 218, 0.5);
      line-height: 1.2;
    }
    .latency-good {
      color: #00ff88;
    }
    .latency-medium {
      color: #ffaa00;
    }
    .latency-bad {
      color: #ff4444;
    }
    .rssi-excellent {
      color: #00ff88;
    }
    .rssi-good {
      color: #64ffda;
    }
    .rssi-fair {
      color: #ffaa00;
    }
    .rssi-weak {
      color: #ff4444;
    }
    .metrics-section {
      margin-top: 28px;
      background: rgba(15, 52, 96, 0.3);
      border: 1px solid rgba(0, 212, 255, 0.25);
      padding: 25px;
      border-radius: 18px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
    }
    .metrics-title {
      color: #00d4ff;
      font-size: 22px;
      margin-bottom: 18px;
      text-align: center;
      text-shadow: 0 0 18px rgba(0, 212, 255, 0.5);
      letter-spacing: 1px;
      font-weight: 700;
    }
    .metrics-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
      gap: 15px;
      max-width: 100%;
    }
    @media (min-width: 1000px) {
      .metrics-grid {
        grid-template-columns: repeat(4, 1fr);
        gap: 15px;
      }
    }
    .metric-item {
      background: linear-gradient(135deg, rgba(100, 255, 218, 0.1) 0%, rgba(0, 212, 255, 0.1) 100%);
      border: 1px solid rgba(100, 255, 218, 0.25);
      padding: 16px 18px;
      border-radius: 12px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      transition: all 0.3s ease;
      box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
      min-height: 60px;
    }
    .metric-item:hover {
      transform: translateY(-3px);
      border-color: rgba(100, 255, 218, 0.45);
      box-shadow: 0 6px 20px rgba(100, 255, 218, 0.25);
    }
    .metric-label {
      color: #8892b0;
      font-size: 12px;
      font-weight: 600;
      letter-spacing: 0.8px;
      text-transform: uppercase;
    }
    .metric-value {
      color: #64ffda;
      font-size: 17px;
      font-weight: bold;
      text-shadow: 0 0 8px rgba(100, 255, 218, 0.4);
    }
    .footer {
      text-align: center;
      margin-top: 30px;
      color: #8892b0;
      font-size: 11px;
      line-height: 1.8;
      padding-top: 25px;
      border-top: 1px solid rgba(255, 255, 255, 0.15);
      opacity: 0.8;
    }
    .footer a {
      color: #00d4ff;
      text-decoration: none;
      transition: all 0.3s ease;
    }
    .footer a:hover {
      color: #64ffda;
      text-shadow: 0 0 10px rgba(100, 255, 218, 0.4);
    }
    @media (max-width: 768px) {
      .container {
        padding: 25px 20px;
        max-width: 95%;
      }
      h1 {
        font-size: 26px;
      }
      .sensor-value {
        font-size: 28px;
      }
      .sensor-unit {
        font-size: 18px;
      }
      .sensor-grid {
        grid-template-columns: repeat(2, 1fr);
        gap: 12px;
      }
      .info-section {
        grid-template-columns: repeat(2, 1fr);
        gap: 10px;
      }
      .info-value {
        font-size: 18px;
      }
      .metrics-grid {
        grid-template-columns: 1fr;
        gap: 10px;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🌦️ ESP32 Outdoor Weather Station</h1>
    <div class="subtitle">West Pomeranian Voivodeship, Poland</div>
    <div class="status" id="status">Loading data...</div>

    <div class="info-section">
      <div class="info-card">
        <div class="info-label">System Uptime</div>
        <div class="info-value" id="uptime">--</div>
      </div>
      <div class="info-card">
        <div class="info-label">WiFi Signal</div>
        <div class="info-value" id="wifiSignal">-- dBm</div>
      </div>
      <div class="info-card">
        <div class="info-label">API Latency</div>
        <div class="info-value" id="latency">-- ms</div>
      </div>
      <div class="info-card">
        <div class="info-label">Last Update</div>
        <div class="info-value" id="lastUpdate">--</div>
      </div>
      <div class="info-card">
        <div class="info-label">Next Update</div>
        <div class="info-value" id="nextUpdate">--s</div>
      </div>
    </div>

    <div class="sensor-grid">
      <div class="sensor-card">
        <div class="sensor-label">Temperature</div>
        <div>
          <span class="sensor-value" id="temp">--</span>
          <span class="sensor-unit">°C</span>
        </div>
      </div>
      <div class="sensor-card">
        <div class="sensor-label">Humidity</div>
        <div>
          <span class="sensor-value" id="hum">--</span>
          <span class="sensor-unit">%</span>
        </div>
      </div>
      <div class="sensor-card">
        <div class="sensor-label">Pressure</div>
        <div>
          <span class="sensor-value" id="press">--</span>
          <span class="sensor-unit">hPa</span>
        </div>
      </div>
      <div class="sensor-card">
        <div class="sensor-label">Light</div>
        <div>
          <span class="sensor-value" id="light">--</span>
          <span class="sensor-unit">lx</span>
        </div>
      </div>
    </div>

    <div class="metrics-section">
      <h3 class="metrics-title">Calculated Metrics</h3>
      <div class="metrics-grid">
        <div class="metric-item">
          <span class="metric-label">Dew Point</span>
          <span class="metric-value" id="dewPoint">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Feels Like</span>
          <span class="metric-value" id="feelsLike">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Abs. Humidity</span>
          <span class="metric-value" id="absHumidity">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Comfort Level</span>
          <span class="metric-value" id="comfort">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Air Quality</span>
          <span class="metric-value" id="airQuality">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Pressure Trend</span>
          <span class="metric-value" id="pressureTrend">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Forecast</span>
          <span class="metric-value" id="forecast">--</span>
        </div>
        <div class="metric-item">
          <span class="metric-label">Moon Phase</span>
          <span class="metric-value" id="moonPhase">--</span>
        </div>
      </div>
    </div>

    <div class="footer">
      Auto-refresh every 5 seconds.<br>
      Copyright (c) 2025 Sefinek. Licensed under MIT.
    </div>
  </div>
  <script>
    const formatUptime = (seconds) => {
      const days = Math.floor(seconds / 86400);
      const hours = Math.floor((seconds % 86400) / 3600);
      const minutes = Math.floor((seconds % 3600) / 60);
      const secs = seconds % 60;

      const result = [];
      if (days > 0) result.push(`${days}d`);
      if (hours > 0) result.push(`${hours}h`);
      if (minutes > 0) result.push(`${minutes}m`);
      result.push(`${secs}s`);

      return result.join(' ');
    };

    const calculateDewPoint = (temp, humidity) => {
      if (temp === undefined || temp === null || isNaN(temp)) return 'N/A';
      if (!humidity || isNaN(humidity)) return 'N/A';
      const a = 17.27;
      const b = 237.7;
      const alpha = ((a * temp) / (b + temp)) + Math.log(humidity / 100);
      const dewPoint = (b * alpha) / (a - alpha);
      return `${dewPoint.toFixed(1)}°C`;
    };

    const calculateFeelsLike = (temp, humidity) => {
      if (temp === undefined || temp === null || isNaN(temp)) return 'N/A';
      if (!humidity || isNaN(humidity)) return `${temp.toFixed(1)}°C`;
      if (temp >= 27) {
        const hi = -8.78469475556 + 1.61139411 * temp + 2.33854883889 * humidity +
                    -0.14611605 * temp * humidity + -0.012308094 * temp * temp +
                    -0.0164248277778 * humidity * humidity + 0.002211732 * temp * temp * humidity +
                    0.00072546 * temp * humidity * humidity + -0.000003582 * temp * temp * humidity * humidity;
        return `${hi.toFixed(1)}°C`;
      }
      if (temp <= 10) {
        const windChill = temp - 2;
        return `${windChill.toFixed(1)}°C`;
      }
      return `${temp.toFixed(1)}°C`;
    };

    const calculateAbsoluteHumidity = (temp, humidity) => {
      if (temp === undefined || temp === null || isNaN(temp)) return 'N/A';
      if (!humidity || isNaN(humidity)) return 'N/A';
      const absHumidity = (6.112 * Math.exp((17.67 * temp) / (temp + 243.5)) * humidity * 2.1674) / (273.15 + temp);
      return `${absHumidity.toFixed(1)} g/m³`;
    };

    const getComfortLevel = (temp, humidity) => {
      if (temp === undefined || temp === null || isNaN(temp)) return 'N/A';
      if (!humidity || isNaN(humidity)) {
        if (temp < 15) return '🥶 Cold';
        if (temp > 25) return '🥵 Hot';
        return '😊 OK';
      }
      if (temp >= 20 && temp <= 26 && humidity >= 40 && humidity <= 60) return '😊 Comfortable';
      if (temp < 15) return '🥶 Cold';
      if (temp > 30) return '🥵 Hot';
      if (humidity < 30) return '🏜️ Dry';
      if (humidity > 70) return '💧 Humid';
      return '😐 Moderate';
    };

    const getAirQuality = humidity => {
      if (!humidity || isNaN(humidity)) return 'N/A';
      if (humidity >= 40 && humidity <= 60) return '✅ Optimal';
      if (humidity < 30) return '⚠️ Too Dry';
      if (humidity > 70) return '⚠️ Too Humid';
      return '~ Fair';
    };

    const getPressureTrend = currentPressure => {
      const history = JSON.parse(localStorage.getItem('pressureHistory') || '[]');
      history.push({ p: currentPressure, t: Date.now() });
      const cutoff = Date.now() - 3 * 60 * 60 * 1000;
      const filtered = history.filter(h => h.t > cutoff);
      localStorage.setItem('pressureHistory', JSON.stringify(filtered.slice(-50)));
      if (filtered.length < 2) return '− Stable';
      const oldest = filtered[0].p;
      const diff = currentPressure - oldest;
      if (diff > 300) return '↗ Rising';
      if (diff < -300) return '↘ Falling';
      return '− Stable';
    };

    const getForecast = (pressureTrend) => {
      if (pressureTrend.includes('Rising')) return '☀️ Improving';
      if (pressureTrend.includes('Falling')) return '🌧️ Worsening';
      return '− Stable';
    };

    let moonPhaseCache = null;
    let moonPhaseCacheDay = -1;

    const getMoonPhase = () => {
      const now = new Date();
      const today = now.getDate();
      if (moonPhaseCache && moonPhaseCacheDay === today) return moonPhaseCache;
      const year = now.getFullYear();
      const month = now.getMonth() + 1;
      let jd = 367 * year - Math.floor(7 * (year + Math.floor((month + 9) / 12)) / 4) +
               Math.floor(275 * month / 9) + today + 1721013.5;
      const daysSinceNew = (jd - 2451549.5) % 29.53;
      let phase = '🌘 Waning';
      if (daysSinceNew < 1.84) phase = '🌑 New';
      else if (daysSinceNew < 7.38) phase = '🌒 Waxing';
      else if (daysSinceNew < 9.23) phase = '🌓 First Q';
      else if (daysSinceNew < 14.77) phase = '🌔 Waxing G';
      else if (daysSinceNew < 16.61) phase = '🌕 Full';
      else if (daysSinceNew < 22.15) phase = '🌖 Waning G';
      else if (daysSinceNew < 23.99) phase = '🌗 Last Q';
      moonPhaseCache = phase;
      moonPhaseCacheDay = today;
      return phase;
    };

    const colors = {
      blue: { c: '#3b82f6', s: 'rgba(59, 130, 246, 0.6)' },
      lightBlue: { c: '#60a5fa', s: 'rgba(96, 165, 250, 0.6)' },
      teal: { c: '#34d399', s: 'rgba(52, 211, 153, 0.6)' },
      green: { c: '#10b981', s: 'rgba(16, 185, 129, 0.6)' },
      yellow: { c: '#fbbf24', s: 'rgba(251, 191, 36, 0.6)' },
      orange: { c: '#f97316', s: 'rgba(249, 115, 22, 0.6)' },
      red: { c: '#ef4444', s: 'rgba(239, 68, 68, 0.6)' },
      purple: { c: '#8b5cf6', s: 'rgba(139, 92, 246, 0.6)' },
      indigo: { c: '#6366f1', s: 'rgba(99, 102, 241, 0.6)' },
      cyan: { c: '#00d4ff', s: 'rgba(0, 212, 255, 0.5)' }
    };

    const getSignalQuality = rssi => rssi >= -55 ? 'rssi-excellent' : rssi >= -67 ? 'rssi-good' : rssi >= -75 ? 'rssi-fair' : 'rssi-weak';

    const getTempColor = t => t < 0 ? colors.blue : t < 10 ? colors.lightBlue : t < 18 ? colors.teal : t < 26 ? colors.green : t < 30 ? colors.yellow : t < 35 ? colors.orange : colors.red;

    const getHumidityColor = h => h < 20 ? colors.red : h < 30 ? colors.orange : h < 40 ? colors.yellow : h < 60 ? colors.green : h < 70 ? colors.lightBlue : colors.blue;

    const getPressureColor = p => p < 980 ? colors.purple : p < 1000 ? colors.lightBlue : p < 1020 ? colors.green : p < 1040 ? colors.yellow : colors.orange;

    const getLightColor = l => l < 10 ? colors.indigo : l < 100 ? colors.purple : l < 1000 ? colors.green : l < 10000 ? colors.yellow : colors.orange;

    const getUptimeColor = s => s < 60 ? colors.red : s < 300 ? colors.orange : s < 3600 ? colors.yellow : s < 86400 ? colors.green : colors.cyan;

    const getCountdownColor = s => s <= 0 ? colors.red : s <= 2 ? colors.yellow : colors.green;

    const applyColor = (el, c) => {
      el.style.color = c.c;
      el.style.textShadow = `0 0 12px ${c.s}`;
    };

    let countdown = 5;

    const updateData = () => {
      const statusEl = document.getElementById('status');
      const latencyEl = document.getElementById('latency');
      const startTime = performance.now();

      fetch('/api/v1/sensors')
        .then(res => {
          const latency = Math.round(performance.now() - startTime);
          return res.json().then(data => ({ data, latency }));
        })
        .then(({ data, latency }) => {
          const temp = data.temperature;
          const hum = data.humidity;
          const press = data.pressure;
          const rssi = data.rssi;

          const tempEl = document.getElementById('temp');
          const humEl = document.getElementById('hum');
          const pressEl = document.getElementById('press');
          const lightEl = document.getElementById('light');

          if (temp !== undefined && temp !== null) {
            tempEl.textContent = temp.toFixed(2);
            applyColor(tempEl, getTempColor(temp));
          } else {
            tempEl.textContent = 'N/A';
          }

          if (hum !== undefined && hum !== null) {
            humEl.textContent = hum.toFixed(2);
            applyColor(humEl, getHumidityColor(hum));
          } else {
            humEl.textContent = 'N/A';
          }

          if (press !== undefined && press !== null) {
            const pressHpa = press / 100;
            pressEl.textContent = pressHpa.toFixed(2);
            applyColor(pressEl, getPressureColor(pressHpa));
          } else {
            pressEl.textContent = 'N/A';
          }

          if (data.light !== undefined && data.light !== null) {
            lightEl.textContent = data.light.toFixed(2);
            applyColor(lightEl, getLightColor(data.light));
          } else {
            lightEl.textContent = 'N/A';
          }

          const uptimeEl = document.getElementById('uptime');
          uptimeEl.textContent = formatUptime(data.uptime);
          applyColor(uptimeEl, getUptimeColor(data.uptime));

          const wifiSignalEl = document.getElementById('wifiSignal');
          wifiSignalEl.textContent = `${rssi} dBm`;
          wifiSignalEl.className = `info-value ${getSignalQuality(rssi)}`;

          latencyEl.textContent = `${latency} ms`;
          latencyEl.className = `info-value ${latency < 80 ? 'latency-good' : latency < 180 ? 'latency-medium' : 'latency-bad'}`;

          document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();

          const pressureTrend = (press !== undefined && press !== null) ? getPressureTrend(press) : '− N/A';
          document.getElementById('dewPoint').textContent = calculateDewPoint(temp, hum);
          document.getElementById('feelsLike').textContent = calculateFeelsLike(temp, hum);
          document.getElementById('absHumidity').textContent = calculateAbsoluteHumidity(temp, hum);
          document.getElementById('comfort').textContent = getComfortLevel(temp, hum);
          document.getElementById('airQuality').textContent = getAirQuality(hum);
          document.getElementById('pressureTrend').textContent = pressureTrend;
          document.getElementById('forecast').textContent = getForecast(pressureTrend);
          document.getElementById('moonPhase').textContent = getMoonPhase();

          countdown = 5;

          statusEl.textContent = '✅ System Online';
          statusEl.className = 'status';
        })
        .catch(() => {
          statusEl.textContent = '❌ Connection Error';
          statusEl.className = 'status error';
          latencyEl.textContent = '-- ms';
          latencyEl.className = 'info-value';
        });
    };

    setInterval(() => {
      countdown--;
      if (countdown < 0) countdown = 0;
      const nextUpdateEl = document.getElementById('nextUpdate');
      nextUpdateEl.textContent = `${countdown}s`;
      applyColor(nextUpdateEl, getCountdownColor(countdown));
    }, 1000);

    updateData();
    setInterval(updateData, 5000);
  </script>
</body>
</html>
)rawliteral";

#endif
