#include <Wire.h>                     // Include Wire library for I2C communication
#include <Adafruit_PWMServoDriver.h>  // Include Adafruit PWM Servo Driver library

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();  // Create PWM servo driver object

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 325  // Maximum pulse length count (out of 4096)
// fat fart
#define sawServoChannel 2             // Define servo channel for saw servo
#define sweepServoChannel 8           // Define servo channel for sweep servo
#define chopsawServoChannel 1         // Define servo channel for chop saw
#define cncServoChannel 3             // Define servo channel for CNC
#define downdrafttableServoChannel 7  // Define servo channel for downdraft table

int chopsawswitchPin = 6;         // Pin the chop saw switch is connected to
int cncswitchPin = 8;             // Pin the CNC switch is connected to
int downdrafttableSwitchPin = 9;  // Pin the downdraft table switch is connected to
int sawswitchPin = 5;             // Pin the saw switch is connected to
int sweepswitchPin = 4;           // Pin the sweep switch is connected to

const int dustCollectionRelayPin = 7;   // Dust collection relay pin
const int chopsawRelayPin = 10;         // Chop saw relay pin
const int cncRelayPin = 12;             // CNC relay pin
const int downdrafttableRelayPin = 13;  // Downdraft table relay pin
const int tablesawRelayPin = 3;         // Table saw relay pin

int chopsawswitchstate;    // State of the chop saw switch
int cncswitchstate;        // State of the CNC switch
int downdraftswitchstate;  // State of the downdraft table switch
int sawswitchState;        // State of the saw switch
int sweepswitchState;      // State of the sweep switch

boolean collectorIsOn = false;       // Dust collector state
boolean tablesawIsOn = false;        // Table saw state
boolean chopsawIsOn = false;         // Chop saw state
boolean cncIsOn = false;             // CNC state
boolean DownDraftTableIsOn = false;  // Downdraft table state

void setup() {
  pwm.begin();         // Initialize PWM servo driver
  pwm.setPWMFreq(60);  // Set PWM frequency to 60 Hz for analog servos

  pinMode(chopsawRelayPin, OUTPUT);         // Set chop saw relay pin as output
  pinMode(cncRelayPin, OUTPUT);             // Set CNC relay pin as output
  pinMode(downdrafttableRelayPin, OUTPUT);  // Set downdraft table relay pin as output
  pinMode(dustCollectionRelayPin, OUTPUT);  // Set dust collection relay pin as output
  pinMode(tablesawRelayPin, OUTPUT);        // Set table saw relay pin as output
  pinMode(chopsawswitchPin, INPUT);         // Set chop saw switch pin as input
  pinMode(cncswitchPin, INPUT);             // Set CNC switch pin as input
  pinMode(downdrafttableSwitchPin, INPUT);  // Set downdraft table switch pin as input
  pinMode(sweepswitchPin, INPUT);           // Set sweep switch pin as input
  pinMode(sawswitchPin, INPUT);             // Set saw switch pin as input

  digitalWrite(dustCollectionRelayPin, HIGH);  // Initialize dust collection relay to HIGH (off)
  digitalWrite(chopsawRelayPin, HIGH);         // Initialize chop saw relay to HIGH (off)
  digitalWrite(downdrafttableRelayPin, HIGH);  // Initialize downdraft table relay to HIGH (off)

  Serial.begin(9600);              // Initialize serial communication at 9600 baud rate
  delay(15);                       // Short delay to allow setup to stabilize
  Serial.println("Hello World!");  // Print "Hello World!" message to serial monitor

  // Calibrate servos to their initial positions
  pwm.setPWM(sweepServoChannel, 0, SERVOMAX);           // Set sweep servo to open position
  pwm.setPWM(sawServoChannel, 0, SERVOMAX);             // Set saw servo to open position
  pwm.setPWM(chopsawServoChannel, 0, SERVOMAX);         // Set chop saw servo to open position
  pwm.setPWM(cncServoChannel, 0, SERVOMAX);             // Set CNC servo to open position
  pwm.setPWM(downdrafttableServoChannel, 0, SERVOMAX);  // Set downdraft table servo to open position
  delay(1000);                                          // Wait 1 second for servos to reach open position

  pwm.setPWM(sweepServoChannel, 0, SERVOMIN);           // Set sweep servo to closed position
  pwm.setPWM(sawServoChannel, 0, SERVOMIN);             // Set saw servo to closed position
  pwm.setPWM(chopsawServoChannel, 0, SERVOMIN);         // Set chop saw servo to closed position
  pwm.setPWM(cncServoChannel, 0, SERVOMIN);             // Set CNC servo to closed position
  pwm.setPWM(downdrafttableServoChannel, 0, SERVOMIN);  // Set downdraft table servo to closed position
  delay(1000);                                          // Wait 1 second for servos to reach closed position
}

void loop() {
  delay(50);  // Debounce delay to avoid multiple readings for a single switch press

  // Read switch states
  sweepswitchState = digitalRead(sweepswitchPin);               // Read sweep switch state
  chopsawswitchstate = digitalRead(chopsawswitchPin);           // Read chop saw switch state
  cncswitchstate = digitalRead(cncswitchPin);                   // Read CNC switch state
  downdraftswitchstate = digitalRead(downdrafttableSwitchPin);  // Read downdraft table switch state
  sawswitchState = digitalRead(sawswitchPin);                   // Read saw switch state

  // Sweep switch logic
  if (sweepswitchState == LOW) {                 // Sweep switch is pressed
    Serial.println("Sweep Switch ON");           // Print sweep switch ON message to serial monitor
    pwm.setPWM(sweepServoChannel, 0, SERVOMAX);  // Open sweep servo
    delay(3000);                                 // Delay for 3 seconds to allow servo to move

    if (!collectorIsOn) {                         // If dust collector is not already on
      Serial.println("turnOnDustCollection");     // Print turn on dust collection message
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;                       // Set dust collector state to on
    }
  } else {
    delay(3000);                                       // Sweep switch is not pressed
    pwm.setPWM(sweepServoChannel, 0, SERVOMIN);  // Close sweep servo

    if (collectorIsOn && sawswitchState != LOW) {  // If dust collector is on and saw switch is not pressed
      Serial.println("Sweep Switch OFF");          // Print sweep switch OFF message to serial monitor
      Serial.println("turnOffDustCollection");     // Print turn off dust collection message
      digitalWrite(dustCollectionRelayPin, HIGH);  // Turn off dust collection
      collectorIsOn = false;                       // Set dust collector state to off
    }
  }

  // Chop saw switch logic
  if (chopsawswitchstate == LOW) {                 // Chop saw switch is pressed
    Serial.println("Chop Switch ON");              // Print chop saw switch ON message to serial monitor
    pwm.setPWM(chopsawServoChannel, 0, SERVOMAX);  // Open chop saw servo
    delay(3000);                                   // Delay for 3 seconds to allow servo to move

    if (!collectorIsOn) {                         // If dust collector is not already on
      Serial.println("turnOnDustCollection");     // Print turn on dust collection message
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;                       // Set dust collector state to on
    }
  } else {                                         // Chop saw switch is not pressed
    pwm.setPWM(chopsawServoChannel, 0, SERVOMIN);  // Close chop saw servo

    if (collectorIsOn && chopsawswitchstate != LOW) {  // If dust collector is on and chop saw switch is not pressed
      Serial.println("Chop Saw Switch OFF");           // Print chop saw switch OFF message to serial monitor
      Serial.println("turnOffDustCollection");         // Print turn off dust collection message
      digitalWrite(dustCollectionRelayPin, HIGH);      // Turn off dust collection
      collectorIsOn = false;                           // Set dust collector state to off
    }
  }

  // CNC switch logic
  if (cncswitchstate == LOW) {                 // CNC switch is pressed
    Serial.println("CNC Switch ON");           // Print CNC switch ON message to serial monitor
    pwm.setPWM(cncServoChannel, 0, SERVOMAX);  // Open CNC servo
    delay(3000);                               // Delay for 3 seconds to allow servo to move

    if (!collectorIsOn) {                         // If dust collector is not already on
      Serial.println("turnOnDustCollection");     // Print turn on dust collection message
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;                       // Set dust collector state to on
    }
  } else {                                     // CNC switch is not pressed
    pwm.setPWM(cncServoChannel, 0, SERVOMIN);  // Close CNC servo

    if (collectorIsOn && cncswitchstate != LOW) {  // If dust collector is on and CNC switch is not pressed
      Serial.println("CNC Switch OFF");            // Print CNC switch OFF message to serial monitor
      Serial.println("turnOffDustCollection");     // Print turn off dust collection message
      digitalWrite(dustCollectionRelayPin, HIGH);  // Turn off dust collection
      collectorIsOn = false;                       // Set dust collector state to off
    }
  }

  // Saw switch logic
  if (sawswitchState == LOW) {                 // Saw switch is pressed
    Serial.println("Saw Switch ON");           // Print saw switch ON message to serial monitor
    pwm.setPWM(sawServoChannel, 0, SERVOMAX);  // Open saw servo
    delay(15);                                 // Short delay to allow servo to move

    if (!collectorIsOn) {                         // If dust collector is not already on
      Serial.println("turnOnDustCollection");     // Print turn on dust collection message
      digitalWrite(dustCollectionRelayPin, LOW);  // Turn on dust collection
      collectorIsOn = true;                       // Set dust collector state to on
      delay(15);                                  // Short delay to allow relay to activate
    }
  } else {                                     // Saw switch is not pressed
    pwm.setPWM(sawServoChannel, 0, SERVOMIN);  // Close saw servo
    delay(15);                                 // Short delay to allow servo to move

    if (collectorIsOn && sweepswitchState != LOW) {  // If dust collector is on and sweep switch is not pressed
      Serial.println("Saw Switch OFF");              // Print saw switch OFF message to serial monitor
      Serial.println("turnOffDustCollection");       // Print turn off dust collection message
      digitalWrite(dustCollectionRelayPin, HIGH);    // Turn off dust collection
      collectorIsOn = false;                         // Set dust collector state to off
      delay(15);                                     // Short delay to allow relay to deactivate
    }
  }
}
