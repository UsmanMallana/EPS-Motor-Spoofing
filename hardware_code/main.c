// --- Motor Driver Pins (L298) ---
// Note: The physical ATMEGA pins 13, 14, and 15 correspond to Arduino pins 9, 10, and 11.
const int ENA = 9;  // ATMEGA Pin 13 (PB1) - PWM Speed
const int IN1 = 10; // ATMEGA Pin 14 (PB2) - Direction 1
const int IN2 = 11; // ATMEGA Pin 15 (PB3) - Direction 2

// --- Steering Angle Sensor (Bypass) ---
const int potPin = A0; // ATMEGA Pin 23 (PC0)

// --- PID Tuning Parameters ---
float Kp = 1.5;  // Proportional (How hard to turn)
float Ki = 0.0;  // Integral (Turned off for simulation simplicity)
float Kd = 0.5;  // Derivative (Braking to prevent overshoot)

// --- The "Autonomous" Command ---
// 512 is exactly 50% on your 10-bit potentiometer (0-1023)
int targetPosition = 512; 

float previousError = 0;
unsigned long previousTime = 0;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  // 1. Time tracking for smooth math
  unsigned long currentTime = millis();
  float dt = (currentTime - previousTime) / 1000.0;
  if (dt <= 0.0) dt = 0.01;
  previousTime = currentTime;

  // 2. Read the current "physical" steering angle (0 to 1023)
  int currentPosition = analogRead(potPin);

  // 3. Calculate how far away we are from the target
  float error = targetPosition - currentPosition;

  // 4. Calculate PID Output
  float P = Kp * error;
  float D = Kd * (error - previousError) / dt;
  float output = P + D;
  previousError = error;

  // 5. Constrain motor speed to Arduino limits (0 to 255)
  int motorSpeed = abs(output);
  if (motorSpeed > 255) motorSpeed = 255;

  // 6. Drive the L298 H-Bridge
  if (abs(error) < 3) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
  else if (output > 0) {
    // Error is positive: Steer Right
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motorSpeed); 
  }
  else {
    // Error is negative: Steer Left
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, motorSpeed); 
  }

  delay(10); 
}