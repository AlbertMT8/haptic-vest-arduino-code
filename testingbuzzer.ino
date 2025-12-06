// ---- Pin Assignments ----
const int buzzer1 = 3;
const int buzzer2 = 4;
const int buzzer3 = 5;

void setup() {
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(buzzer3, OUTPUT);
}

void loop() {
  buzz(buzzer1, 500);  // Buzzer 1 for 0.5 s
  buzz(buzzer2, 500);  // Buzzer 2 for 0.5 s
  buzz(buzzer3, 500);  // Buzzer 3 for 0.5 s
}

// ---- Helper Function ----
// frequency (Hz) default = 2000
// duration (ms)
void buzz(int pin, int duration) {
  tone(pin, 2000);   // Make sound
  delay(duration);
  noTone(pin);       // Stop sound
  delay(200);        // Small pause
}
