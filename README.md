# 🔐🏠 Smart Home Automation System using Arduino Mega

A fully integrated **Smart Home Automation System** powered by the **Arduino Mega 2560**. This project combines embedded systems, security, IoT, and custom app development to create a modern, automated smart home environment.

---

## 📌 Features

### 1. 🔢 Keypad Security System
- Secure 4-digit password entry.
- Allows only **4 failed attempts** before system lockout.
- Grants access and unlocks the door on successful entry.

### 2. 📛 RFID Access Control
- Authorized users can tap RFID cards.
- Access is granted to recognized tags, with the **user’s name displayed** on the LCD.
- Adds another layer of security to the system.

### 3. 🔥 Fire Detection
- Monitors for fire using a flame or smoke sensor.
- Triggers an **alarm** (buzzer or notification) when fire is detected.
- Can be expanded to activate safety mechanisms.

### 4. 🌡️ Temperature Monitoring & Control
- Uses a **DHT11/DHT22** sensor to monitor room temperature.
- Automatically controls fan or cooling systems based on temperature thresholds.

### 5. 📱 Mobile App Automation
Designed using **MIT App Inventor**, this mobile app allows you to remotely control home systems including:

- 🚪 Door Open/Close  
- 💡 Parlor Light ON/OFF  
- 🌙 Security Light ON/OFF  
- 🎵 Music Control (via Buzzer)  
- 🌀 Fan Control  

Communication is done via **Bluetooth (HC-05)** or **Wi-Fi (optional)**.

### 6. ⚡ Power Management System
- Controls connected devices to optimize power usage.
- Can disable non-critical systems in low power conditions.
- Future-ready for solar or inverter integration.

---

## 🛠️ Hardware Components

| Component             | Description                        |
|------------------------|------------------------------------|
| Arduino Mega 2560      | Main microcontroller               |
| Keypad 4x4             | Password entry                     |
| RC522 RFID Module      | RFID authentication                |
| RFID Tags/Cards        | User identification                |
| 16x2 LCD (I2C)         | Displays system feedback           |
| Flame Sensor           | Detects fire/smoke                 |
| DHT11/DHT22            | Temperature and humidity sensing   |
| Relay Modules (4-6)    | Controls appliances (fan, light)   |
| Bluetooth (HC-05)      | Wireless communication             |
| Buzzer                 | Audio alert and music playback     |
| Smartphone             | Runs the MIT App                   |

---

## 🧠 System Overview

