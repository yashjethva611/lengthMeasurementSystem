
#include <Servo.h>
#include <math.h>

Servo scanner;

// Pins
const int servoPin = 9;
const int trigPin  = 10;
const int echoPin  = 11;

// Scan range
const int startAngle = 20;
const int endAngle   = 160;
const int stepAngle  = 1;

// Tuning values
const float objectThreshold = 80.0;   // cm, adjust to your setup
const int samplesPerAngle = 5;
const int sampleDelayMs = 25;

// Edge storage
bool inObject = false;
bool objectFound = false;

float leftAngle = -1;
float leftDistance = 0;

float rightAngle = -1;
float rightDistance = 0;

float readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 25000); // timeout 25 ms
  if (duration == 0) return 999.0;               // no echo

  return (duration * 0.0343) / 2.0;
}

float readAverageDistance() {
  float sum = 0.0;
  int valid = 0;

  for (int i = 0; i < samplesPerAngle; i++) {
    float d = readDistanceCm();
    if (d > 0 && d < 999.0) {
      sum += d;
      valid++;
    }
    delay(10);
  }

  if (valid == 0) return 999.0;
  return sum / valid;
}

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  scanner.attach(servoPin);
  scanner.write(90);
  delay(1000);

  Serial.println("Starting object width scan...");
}

void loop() {
  // Reset edge data for each scan
  inObject = false;
  objectFound = false;
  leftAngle = -1;
  rightAngle = -1;

  float previousDistance = 999.0;

  // Sweep the servo
  for (int a = startAngle; a <= endAngle; a += stepAngle) {
    scanner.write(a);
    delay(sampleDelayMs);

    float d = readAverageDistance();

    Serial.print("Angle: ");
    Serial.print(a);
    Serial.print(" deg, Distance: ");
    Serial.print(d);
    Serial.println(" cm");

    // Simple threshold-based detection
    if (!inObject && d < objectThreshold) {
      // First time we enter the object region = left edge
      inObject = true;
      objectFound = true;
      leftAngle = a;
      leftDistance = d;
    }

    if (inObject && d >= objectThreshold) {
      // We just left the object region = right edge
      rightAngle = a - stepAngle;
      rightDistance = previousDistance;
      inObject = false;
      break; // stop scanning after object ends
    }

    previousDistance = d;
  }

  // Handle case where object extends to the last scanned angle
  if (inObject && objectFound) {
    rightAngle = endAngle;
    rightDistance = previousDistance;
  }

  if (!objectFound || leftAngle < 0 || rightAngle < 0) {
    Serial.println("No object detected.");
    Serial.println("-----");
    delay(2000);
    return;
  }

  // Convert polar coordinates to Cartesian
  float x1 = leftDistance * cos(radians(leftAngle));
  float y1 = leftDistance * sin(radians(leftAngle));

  float x2 = rightDistance * cos(radians(rightAngle));
  float y2 = rightDistance * sin(radians(rightAngle));

  // Best practical estimate for a front-facing object
  float widthFlat = abs(y2 - y1);

  // Optional straight-line distance between edge points
  float widthEdgeToEdge = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

  Serial.println();
  Serial.println("Object detected!");
  Serial.print("Left edge  : angle=");
  Serial.print(leftAngle);
  Serial.print(" deg, distance=");
  Serial.print(leftDistance);
  Serial.println(" cm");

  Serial.print("Right edge : angle=");
  Serial.print(rightAngle);
  Serial.print(" deg, distance=");
  Serial.print(rightDistance);
  Serial.println(" cm");

  Serial.print("Estimated width = ");
  Serial.print(widthFlat);
  Serial.println(" cm");

  Serial.print("Edge-to-edge distance = ");
  Serial.print(widthEdgeToEdge);
  Serial.println(" cm");
  Serial.println("-----");

  delay(3000);
}