#include <SoftwareSerial.h>
#include <Servo.h>

// ===== TF-Luna (UART) =====
// TF-Luna TX -> D10 (Arduino RX), GND -> GND, 5V -> 5V
SoftwareSerial lidar(10, 11);  // RX=10, TX=11 (TX unused)

const long   LIDAR_BAUD = 115200;
const int    LED_PIN    = LED_BUILTIN;
const uint16_t THRESH_CM = 100;   // Detection threshold

// ===== Servo (SG90) =====
Servo servo9g;
const int SERVO_PIN = 9;          // SG90 signal wire on D9
const float SERVO_MIN = 30.0;     // -60° relative to center
const float SERVO_MAX = 150.0;    // +60° relative to center

// Sweep: 30° -> 150° -> 30° = 240° in 1 second
// => speed = 240 deg / 1 s = 240 deg/s
const float SERVO_SPEED_DEG_PER_SEC = 240.0;

// ===== Buzzers =====
// Red wires: D3, D4, D5. Black wires: GND / GND rail.
const int BUZZER_LEFT   = 3;      // for angles near 30°
const int BUZZER_MIDDLE = 4;      // for angles near 90°
const int BUZZER_RIGHT  = 5;      // for angles near 150°

const int BUZZ_FREQ = 2000;             // Hz
const unsigned long BUZZ_DURATION_MS = 80; // quick beep

// ---- TF-Luna parser (UART) ----
bool readTFLuna(uint16_t &distance_cm) {
  lidar.listen();
  static uint8_t state = 0;
  static uint8_t buf[9];
  static uint8_t idx = 0;

  while (lidar.available()) {
    uint8_t b = (uint8_t)lidar.read();

    switch (state) {
      case 0:
        if (b == 0x59) {
          state = 1;
          buf[0] = b;
        }
        break;

      case 1:
        if (b == 0x59) {
          state = 2;
          buf[1] = b;
          idx = 2;
        } else {
          state = 0;
        }
        break;

      case 2:
        buf[idx++] = b;
        if (idx >= 9) {
          state = 0;
          idx = 0;

          uint16_t sum = 0;
          for (int i = 0; i < 8; i++) sum += buf[i];

          if ((uint8_t)sum == buf[8]) {
            distance_cm = ((uint16_t)buf[3] << 8) | buf[2];
            return true;
          }
        }
        break;
    }
  }
  return false;
}

// ---- Buzzer helper: pick buzzer closest to angle ----
// 30° → Left, 90° → Middle, 150° → Right
int buzzerPinForAngle(int angle) {
  if (angle < 60) {
    return BUZZER_LEFT;           // closer to 30°
  } else if (angle < 120) {
    return BUZZER_MIDDLE;         // closer to 90°
  } else {
    return BUZZER_RIGHT;          // closer to 150°
  }
}

const char* buzzerNameForPin(int pin) {
  if (pin == BUZZER_LEFT)   return "Left buzzer";
  if (pin == BUZZER_MIDDLE) return "Middle buzzer";
  if (pin == BUZZER_RIGHT)  return "Right buzzer";
  return "None";
}

// ---- Servo sweep state ----
float currentAngle = 90.0;        // start at center
int sweepDirection = 1;           // +1 going up, -1 going down
unsigned long lastServoUpdate = 0;

// ---- Buzzer state (non-blocking beep) ----
int activeBuzzerPin = -1;         // -1 = none
unsigned long buzzEndTime = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUZZER_LEFT,   OUTPUT);
  pinMode(BUZZER_MIDDLE, OUTPUT);
  pinMode(BUZZER_RIGHT,  OUTPUT);

  Serial.begin(115200);
  lidar.begin(LIDAR_BAUD);
  delay(100);

  servo9g.attach(SERVO_PIN);
  servo9g.write((int)currentAngle);

  Serial.println("TF-Luna + Servo + Buzzers system ready.");
  Serial.print("Threshold (cm): "); Serial.println(THRESH_CM);
}

// ---- Update servo for constant-speed sweep ----
void updateServo() {
  unsigned long now = millis();
  if (lastServoUpdate == 0) {
    lastServoUpdate = now;
    return;
  }

  unsigned long dt_ms = now - lastServoUpdate;
  lastServoUpdate = now;

  // Convert time delta to angle change
  float dt_sec = dt_ms / 1000.0;
  float deltaAngle = SERVO_SPEED_DEG_PER_SEC * dt_sec * sweepDirection;

  currentAngle += deltaAngle;

  // Handle bounds and reverse direction
  if (currentAngle >= SERVO_MAX) {
    currentAngle = SERVO_MAX;
    sweepDirection = -1;
  } else if (currentAngle <= SERVO_MIN) {
    currentAngle = SERVO_MIN;
    sweepDirection = 1;
  }

  servo9g.write((int)currentAngle);
}

// ---- Update buzzer state (stop beep when time is up) ----
void updateBuzzer() {
  unsigned long now = millis();
  if (activeBuzzerPin != -1 && now >= buzzEndTime) {
    noTone(activeBuzzerPin);
    activeBuzzerPin = -1;
  }
}

void loop() {
  // 1) Keep servo sweeping at constant speed
  updateServo();

  // 2) LiDAR reading
  uint16_t d;
  if (readTFLuna(d)) {
    bool inRange = (d >= 20 && d <= 500 && d <= THRESH_CM);
    digitalWrite(LED_PIN, inRange ? HIGH : LOW);

    int angleInt = (int)currentAngle;
    int chosenBuzzer = buzzerPinForAngle(angleInt);

    // 3) If object detected and not currently beeping, start a quick beep
    if (inRange && activeBuzzerPin == -1) {
      activeBuzzerPin = chosenBuzzer;
      tone(activeBuzzerPin, BUZZ_FREQ);
      buzzEndTime = millis() + BUZZ_DURATION_MS;
    }

    // Debug info to Serial
    Serial.print("dist_cm=");
    Serial.print(d);
    Serial.print("  status=");
    Serial.print(inRange ? "DETECTED" : "CLEAR");
    Serial.print("  angle=");
    Serial.print(angleInt);
    Serial.print("  buzzer=");
    Serial.println(inRange ? buzzerNameForPin(chosenBuzzer) : "None");
  }

  // 4) Handle buzzer timing (stop beeps on time)
  updateBuzzer();

  // Small delay to avoid hammering CPU but keep it responsive
  delay(2);
}
