#include <Servo.h> 
int trigPin = 12; int echoPin = 11;
int servoPin = 10; 
Servo s;
int center = 90; int RL = 160; int LL = 20; 
float rmin = 999; float endVal = 60;     
//declarations
float d()   
{
  digitalWrite(trigPin, LOW);   
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);   
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  unsigned long t = pulseIn(echoPin, HIGH);   
  float dist = t * 0.01715; return dist;
}//measuring distance with sensor     
float scan(bool side1, float &minD) 
{ 
  int angle = center;
  int step = side1 ? +2 : -2;
  int lim = side1 ? RL : LL; 
  while (true) 
  {
    angle += step; 
    angle = constrain(angle, LL, RL);      
    s.write(angle);
    delay(60); 
    float dist = d();  //scan continued;
   if (dist > 0 && dist < minD) minD = dist; 
    if (dist > 0 && dist < (endVal + 4)) 
    return dist; 
    Serial.println(angle);
    Serial.println(dist);
    else if (dist > 0) 
    Serial.println("object too long");
    if (angle == lim) Serial.println("object too   
long"); 
   }
}   /*scanning function, sides are differentiated by bool function; edge detected if distance measured increase 4 cm after one two deg turn*/
void setup() 
{ 
  Serial.begin(9600); 
  pinMode(trigPin, OUTPUT);     
  pinMode(echoPin, INPUT);
  s.attach(servoPin); 
} //setup
void loop()
{ 
  rmin = 999; 
  s.write(center); 
  delay(60); 
  float r1 = scan(true, rmin); 
  s.write(center); 
  delay(60); 
  float r2 = scan(false, rmin); 
  float S = sqrt(r1 * r1 - rmin * rmin) + sqrt(r2 *    r2 - rmin * rmin); 
  Serial.println(S); 
} /* loop function, first side is true and other side is false*/
