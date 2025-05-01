#include <Servo.h>
#include <SoftwareSerial.h>

// Motor pins (same as before)
#define ENA 5
#define ENB 6
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Ultrasonic pins (same as before)
#define trigPin 12
#define echoPin 13

// Soil moisture sensor pin
#define soilSensorPin A0

// Servo control pin
#define servoPin 3

// Bluetooth setup (SoftwareSerial on pins 2 and 4)
SoftwareSerial BTSerial(2, 4); // RX, TX for Bluetooth communication

// Variables
long duration;
int distance;
int leftDistance, rightDistance, frontDistance;
int soilMoistureValue;

// Create servo object
Servo myServo;

void setup() {
  // Motor setup (same as before)
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic setup (same as before)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Soil sensor setup (no servo needed)
  pinMode(soilSensorPin, INPUT);

  // Servo setup
  myServo.attach(servoPin);

  // Start motors stopped
  stopMotors();

  // Start Serial for debugging (optional)
  Serial.begin(9600);
  BTSerial.begin(9600);  // Start Bluetooth communication at 9600 baud rate

  Serial.println("Bluetooth Test Ready");
  BTSerial.println("Bluetooth Test Ready");
}

void loop() {
  // Look forward
  myServo.write(90); // Position for ultrasonic sensor
  delay(500);
  frontDistance = readUltrasonic();

  // Look left
  myServo.write(150); // Position for ultrasonic sensor
  delay(500);
  leftDistance = readUltrasonic();

  // Look right
  myServo.write(30); // Position for ultrasonic sensor
  delay(500);
  rightDistance = readUltrasonic();

  // Reset Servo to center for ultrasonic (90°)
  myServo.write(90);

  // Check soil moisture by just touching the surface
  soilMoistureValue = analogRead(soilSensorPin); // Read soil moisture

  // Decision making for movement
  Serial.print("Front: ");
  Serial.print(frontDistance);
  Serial.print(" | Left: ");
  Serial.print(leftDistance);
  Serial.print(" | Right: ");
  Serial.print(rightDistance);
  Serial.print(" | Soil Moisture: ");
  Serial.println(soilMoistureValue);

  // Example decision (check soil moisture before watering):
  if (soilMoistureValue < 500) {
    // If soil is dry, you can trigger watering (add your water pump logic here)
    Serial.println("Soil is dry, consider watering.");
  }

  // Check if Bluetooth is receiving any data
  if (BTSerial.available()) {
    char command = BTSerial.read();  // Read Bluetooth command
    handleBluetoothCommand(command); // Handle the command
  }

  // Decision making for moving the robot
  if (frontDistance > 20) {
    moveForward();
  } else {
    stopMotors();
    delay(300);

    if (leftDistance > rightDistance) {
      turnLeft();
    } else {
      turnRight();
    }

    delay(600);  // Turn delay
  }
}

// Move forward
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

// Turn left
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

// Turn right
void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

// Stop
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Read ultrasonic distance
int readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  return distance;
}

// Handle Bluetooth commands (move, stop, etc.)
void handleBluetoothCommand(char command) {
  switch (command) {
    case 'F': // Move forward
      moveForward();
      break;
    case 'B': // Move backward
      stopMotors();  // Add backward movement logic if needed
      break;
    case 'L': // Turn left
      turnLeft();
      break;
    case 'R': // Turn right
      turnRight();
      break;
    case 'S': // Stop motors
      stopMotors();
      break;
    case 'T': // Test soil moisture
      int soilMoisture = analogRead(soilSensorPin);
      BTSerial.print("Soil Moisture: ");
      BTSerial.println(soilMoisture);
      break;
    default:
      break;
  }
}
