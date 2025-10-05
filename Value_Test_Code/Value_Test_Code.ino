// FlySky Receiver Input Pins
#define CH1_PIN 39  // Forward/Backward
#define CH2_PIN 34  // Left/Right
#define CH5_PIN 35  // Speed Mode Switch

void setup() {
  Serial.begin(115200);  // Start serial communication for debugging

  // Set receiver pins as INPUT
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
  pinMode(CH5_PIN, INPUT);
}

void loop() {
  // Read PWM signals from receiver (timeout 25ms)
  int ch1 = pulseIn(CH1_PIN, HIGH, 25000);  // Forward/Backward
  int ch2 = pulseIn(CH2_PIN, HIGH, 25000);  // Left/Right
  int ch5 = pulseIn(CH5_PIN, HIGH, 25000);  // Speed Mode Switch

  // Print the values on Serial Monitor
  Serial.print("CH1 : " + String(ch1));
  Serial.print("   CH2 : " + String(ch2));
  Serial.print("   CH5 (Speed Mode): " + String(ch5));
  Serial.println();

  delay(50);  // Small delay for readability
}
