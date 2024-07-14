#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 325  // Maximum pulse length count (out of 4096)

#define sawServoChannel 2    // Servo channel for saw servo
#define sweepServoChannel 8  // Servo channel for sweep servo
#define chopsawServoChannel 1 // Servo channel for chop saw 
#define cncServoChannel 3
#define downdrafttableServoChannel 7 

int chopsawswitchPin = 6;
int cncswitchPin = 8;
int downdrafttableSwitchPin = 9;
int sawswitchPin = 5;                  // Pin the saw switch is connected to
int sweepswitchPin = 4;                // Pin the sweep switch is connected to
const int dustCollectionRelayPin = 7;  // Dust collection relay pin
const int chopsawRelayPin = 10;
const int cncRelayPin = 12;
const int downdrafttableRelayPin = 13;
const int tablesawRelayPin = 3;        // Table saw relay pin
int chopsawswitchstate;
int cncswitchstate;
int downdraftswitchstate;
int sawswitchState;                    // State of the saw switch
int sweepswitchState;                  // State of the sweep switch
boolean collectorIsOn = false;         // Dust collector state
boolean tablesawIsOn = false;          // Table saw state
boolean chopsawIsOn = false;
boolean cncIsOn = false;
boolean DownDraftTableIsOn = false;

void setup() {
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pinMode(chopsawRelayPin, OUTPUT);
  pinMode(cncRelayPin, OUTPUT);
  pinMode(downdrafttableRelayPin, OUTPUT);
  pinMode(dustCollectionRelayPin, OUTPUT);
  pinMode(tablesawRelayPin, OUTPUT);
  pinMode(cncswitchPin, INPUT);
  pinMode(downdrafttableSwitchPin, INPUT);
  pinMode(sweepswitchPin, INPUT);
  pinMode(sawswitchPin, INPUT);
  digitalWrite(dustCollectionRelayPin, HIGH);  // Initialize dust collection relay to HIGH (off)
  digitalWrite(tablesawRelayPin, HIGH);        // Initialize table saw relay to HIGH (off)
  digitalWrite(chopsawRelayPin, HIGH);
  digitalWrite(cncRelayPin, HIGH);
  digitalWrite(downdrafttableRelayPin, HIGH);

  Serial.begin(9600);
  delay(15);
  Serial.println("Hello World!");

  // Calibrate servos
  pwm.setPWM(sweepServoChannel, 0, SERVOMAX);  // Open position for sweep servo
  pwm.setPWM(sawServoChannel, 0, SERVOMAX);    // Open position for saw servo
  pwm.setPWM(chopsawServoChannel, 0, SERVOMAX);
  pwm.setPWM(cncServoChannel, 0, SERVOMAX);
  pwm.setPWM(downdrafttableServoChannel, 0, SERVOMAX)
  delay(1000);                                 // Wait for servos to reach open position
  pwm.setPWM(sweepServoChannel, 0, SERVOMIN);  // Closed position for sweep servo
  pwm.setPWM(sawServoChannel, 0, SERVOMIN);    // Closed position for saw servo
  pwm.setPWM(chopsawServoChannel, 0, SERVOMIN);
  pwm.setPWM(cncServoChannel, 0, SERVOMIN);
  pwm.setPWM(downdrafttableServoChannel, 0, SERVOMIN)
  delay(1000);
                                   // Wait for servos to reach closed position
}

void loop() {
  delay(50);  // Debounce delay

  // Read switch states
  sweepswitchState = digitalRead(sweepswitchPin);
  chopsawswitchstate = digitalRead(chopsawswitchPin);
  cncswitchstate = digitalRead(cncswitchPin);
  chopsawswitchstate = digitalRead(chopsawswitchPin);
  sawswitchState = digitalRead(sawswitchPin);

  // Sweep switch logic
  if (sweepswitchState == LOW) {  // Sweep switch is pressed
    Serial.println("Sweep Switch ON");
    pwm.setPWM(sweepServoChannel, 0, SERVOMAX);  // Open sweep servo
    delay(3000);                                 // Delay for servo to move
    if (!collectorIsOn) {
      Serial.println("turnOnDustCollection");
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;
    }
  } else {                                       // Sweep switch is not pressed
    pwm.setPWM(sweepServoChannel, 0, SERVOMIN);  // Close sweep servo
    if (collectorIsOn && sawswitchState != LOW) {
      Serial.println("Sweep Switch OFF");
      Serial.println("turnOffDustCollection");
      digitalWrite(dustCollectionRelayPin, HIGH);  // Turn off dust collection
      collectorIsOn = false;
    }
  }
  

  // Saw switch logic
  if (sawswitchState == LOW) {  // Saw switch is pressed
    Serial.println("Saw Switch ON");
    pwm.setPWM(sawServoChannel, 0, SERVOMAX);  // Open saw servo
    digitalWrite(tablesawRelayPin, LOW);       // Turn on table saw
    delay(15);                                 // Wait for servo to move
    if (!collectorIsOn) {
      Serial.println("turnOnDustCollection");
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;
      digitalWrite(tablesawRelayPin, LOW);  // Ensure table saw relay is on
      delay(15);                            // Wait for relay to activate
    }
  } else {                                     // Saw switch is not pressed
    pwm.setPWM(sawServoChannel, 0, SERVOMIN);  // Close saw servo
    digitalWrite(tablesawRelayPin, HIGH);      // Turn off table saw
    delay(15);                                 // Wait for servo to move
    if (collectorIsOn && sweepswitchState != LOW) {
      Serial.println("Saw Switch OFF");
      Serial.println("turnOffDustCollection");
      digitalWrite(dustCollectionRelayPin, HIGH);  // Turn off dust collection
      digitalWrite(tablesawRelayPin, HIGH);        // Ensure table saw relay is off
      collectorIsOn = false;
      delay(15);  // Wait for relay to deactivate
    }
  }
}
