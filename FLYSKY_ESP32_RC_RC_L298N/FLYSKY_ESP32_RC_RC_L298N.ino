// FlySky Receiver Input Pins
#define CH1_PIN 35  //Forward/Backward
#define CH2_PIN 34  // Left/Right
#define CH5_PIN 39  // Speed Mode toggle Switch

// L298N Right Motor Pins
#define R_IN1 27  //IN1
#define R_IN2 26  //IN2
// L298N Left Motor Pins
#define L_IN1 25  //IN3
#define L_IN2 33  //IN4

#define R_EN 14  // PWM pin (EnA)
#define L_EN 32  // PWM pin (EnB)

// PWM Channels
#define L_EN_CH 0
#define R_EN_CH 1

float speedMultiplier = 1.0;  //Initially Full Speed

// Deadband function
int applyDeadband(int value, int threshold = 10) {
  return (abs(value) < threshold) ? 0 : value;
}

void setup() {
  Serial.begin(115200);

  // Set receiver pins
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
  pinMode(CH5_PIN, INPUT);

  // Set motor control pins
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);

  // Setup PWM channels
  ledcSetup(L_EN_CH, 1000, 8);
  ledcAttachPin(L_EN, L_EN_CH);

  ledcSetup(R_EN_CH, 1000, 8);
  ledcAttachPin(R_EN, R_EN_CH);

  stopMotors();
}

void loop() {
  // Read FlySky PWM signals
  int ch1 = pulseIn(CH1_PIN, HIGH, 25000);
  int ch2 = pulseIn(CH2_PIN, HIGH, 25000);
  int ch5 = pulseIn(CH5_PIN, HIGH, 25000);

  // Debug output
  //Serial.print("CH1: ");
  //Serial.print(ch1);
  // Serial.print(" CH2: ");
  // Serial.println(ch2);

  //prevent unnecessary movement when transmitter is off / not connect with TX
  //the bot will be active only when the channel value within 1000-2000.
  if (ch1 < 500 || ch1 > 2200 || ch2 < 500 || ch2 > 2200) {
    stopMotors();
    return;
  }

  // Speed Mode
  if (ch5 < 1500) {
    speedMultiplier = 1.0;  //100% Speed
  } else {
    speedMultiplier = 0.5;  //0.5 = 50% Speed, 0.7 = 70%, 0.8 = 80%
  }

  // Map joystick to motor speeds
  int x = map(ch1, 1000, 2000, -255, 255) * speedMultiplier;
  int y = map(ch2, 1000, 2000, -255, 255) * speedMultiplier;

  //apply deadband to avoid small values then no signal from transmitter
  x = applyDeadband(x);
  y = applyDeadband(y);

  //motor mixing with correct reverse logic
  int leftSpeed, rightSpeed;
  if (y <= -150) {  //mixing for backward right and backward left
    leftSpeed = y - x;
    rightSpeed = y + x;
  } else {  //mixing for forward, left, right, backward
    leftSpeed = y + x;
    rightSpeed = y - x;
  }

  // Limit speed range
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // Set motor speeds and directions
  setMotor(L_IN1, L_IN2, L_EN_CH, leftSpeed);
  setMotor(R_IN1, R_IN2, R_EN_CH, rightSpeed);

  // Debug output
  //Serial.print("X: ");
  //Serial.print(x);
  //Serial.print(" Y: ");
  //Serial.print(y);
  //Serial.print(" SpeedMode: ");
  //Serial.print(speedMultiplier);
  // Serial.print("  L: ");
  //Serial.print(leftSpeed);
  //Serial.print(" R: ");
  //Serial.println(rightSpeed);
}

void setMotor(uint8_t IN1, uint8_t IN2, uint8_t EN_CH, int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    ledcWrite(EN_CH, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    ledcWrite(EN_CH, -speed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    ledcWrite(EN_CH, 0);
  }
}

void stopMotors() {
  setMotor(L_IN1, L_IN2, L_EN_CH, 0);
  setMotor(R_IN1, R_IN2, R_EN_CH, 0);
}
