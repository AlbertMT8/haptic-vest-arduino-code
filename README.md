# Seeing-Eye Vest
**A wearable mobility aid that detects obstacles using LiDAR and alerts visually-impaired users with haptic feedback.**

---

![Prototype Photos](https://github.com/AlbertMT8/haptic-vest-arduino-code/blob/main/prototypephotos.png?raw=true)

## Overview
The Seeing-Eye Vest is a wearable assistive device designed to help visually-impaired users navigate safely and independently. Using a TF-Luna LiDAR sensor mounted on a pan-tilt servo, the system continuously scans the environment for nearby obstacles. When an object is detected within a set threshold distance, vibration buzzers activate on the vest to indicate the obstacle’s direction.

This project is designed for NYU Tandon’s **EG-UY 1004: Introduction to Engineering & Design** course.

---
## Team

| Name        | Role                                          |
|-------------|-----------------------------------------------|
| Diego Gao   | Hardware — mounting, wiring, enclosure        |
| Clivia Luo  | Design — user comfort, vest integration       |
| Almira Tepe | Project Manager — deliverables and sprint planning |
| Albert Tsao | Software Lead — Arduino firmware and haptics logic |

Special thanks to **Prof. Paredes** for guidance and support.

---

## Features
- Real-time obstacle detection using a 100 cm LiDAR threshold
- Angular scanning from 30° to 150° using an SG90 servo
- Directional haptic feedback to alert left/center/right collisions
- Lightweight wearable design for comfortable daily use
- Modular hardware system for expansion (multiple sensors, audio alerts)

---

## How It Works
1. Servo sweeps horizontally across a 120° field of view
2. LiDAR continuously measures distances
3. If any measurement is within danger range → detection triggers
4. Based on servo angle, a corresponding buzzer vibrates:
   - Left → left buzzer
   - Center → center buzzer
   - Right → right buzzer
5. LEDs optionally light up for debugging/visual confirmation

---

## Hardware Components

| Component       | Model           | Purpose                       |
|----------------|-----------------|-------------------------------|
| Microcontroller| Arduino Uno R3  | System control and logic      |
| LiDAR Sensor   | TF-Luna (UART)  | Distance measurement          |
| Servo Motor    | SG90            | Sweeping detection angle      |
| Piezo Buzzers  | x3              | Haptic alerts                 |
| LED            | Built-in        | Status indicator              |
| Power          | 5V Battery Pack | Portable wearable usage       |

---

## Wiring Diagram (Summary)

| Arduino Pin | Connected To       |
|------------|--------------------|
| D10        | LiDAR TX (UART RX) |
| D11        | LiDAR RX (unused)  |
| D9         | Servo signal       |
| D3 / D4 / D5 | Buzzers          |
| 5V and GND | Power rails        |

Circuit Diagram:

![Circuit Diagram](https://github.com/AlbertMT8/haptic-vest-arduino-code/blob/main/circuitdiagram.png?raw=true)

---

## Code Structure
```text
/src
 └── seeing_eye_vest.ino      # Main Arduino firmware
/docs
 └── Milestone1_Memo.pdf      # Engineering milestones
/hardware
 ├── wiring_diagram.png
 └── servo_mount.stl


Start
 |
Initialize servo + LiDAR + buzzers
 |
Loop:
 ├─ Sweep servo from 30 → 150 degrees
 ├─ Read LiDAR distance
 ├─ If distance < threshold:
 │       Trigger buzzer based on angle
 └─ Continue scanning
```
Code Flowchart:
![Code Flowchart](https://github.com/AlbertMT8/haptic-vest-arduino-code/blob/main/codeflowchart.png?raw=true)

## Planned Improvements
- Add dual-LiDAR system for vertical sensing
- Integrate machine learning to classify obstacle types
- Add ultrasonic backup at blind angles
- Waterproof enclosure and better wire management
- Rechargeable power system

---

## License
MIT License — free to modify, use, and improve with credit.

---

## Getting Started

### Requirements
- Arduino IDE
- TF-Luna UART library (if using helper driver)

### Uploading
1. Open the `.ino` file in Arduino IDE  
2. Select the correct board and port  
3. Click **Upload**


