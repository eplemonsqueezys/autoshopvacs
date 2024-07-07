#include <Servo.h>

Servo sawservo; // Create sawservo object
Servo sweepservo; // Create sweepservo object

int sawswitchPin = 5; // Pin the saw switch is connected to
int sweepswitchPin = 4; // Pin the sweep switch is connected to
const int dustCollectionRelayPin = 7; // Dust collection relay pin
const int tablesawRelayPin = 3; // Table saw relay pin

int sawswitchState; // The state of the saw switch
int sweepswitchState; // The state of the sweep switch
boolean collectorIsOn = false; // If the collector is on or off
boolean tablesawIsOn = false; // If the table saw is on or off
int pos = 0; // Variable to store the servo position

void setup() {
  sweepservo.attach(2); // Connects the sweepservo to pin 2
  sawservo.attach(6); // Connects the sawservo to pin 6
  pinMode(dustCollectionRelayPin, OUTPUT); // Sets dust collection relay pin as an output
  pinMode(tablesawRelayPin, OUTPUT); // Sets table saw relay pin as an output
  pinMode(sweepswitchPin, INPUT); // Sets sweep switch pin as an input
  pinMode(sawswitchPin, INPUT); // Sets saw switch pin as an input
  digitalWrite(dustCollectionRelayPin, LOW); // Initialize dust collection relay pin to LOW
  digitalWrite(tablesawRelayPin, LOW); // Initialize table saw relay pin to LOW
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  digitalWrite(dustCollectionRelayPin, HIGH); // Set dust collection relay pin to HIGH
  sweepservo.write(90); // moves the sweep servo to closed position
  delay(15); // waits 15 milisecounds to give the servo time to move to the position
  sawservo.write(90); // move the saw servo to closed position
  delay(5000); // Wait for 5 seconds
}

void loop() {
  delay(50); // Delay for 50ms to debounce switches

  sweepswitchState = digitalRead(sweepswitchPin); // Read the state of the sweep switch
  sawswitchState = digitalRead(sawswitchPin); // Read the state of the saw switch

  if (sweepswitchState == LOW) { // If sweep switch is pressed
    Serial.println("Sweep Switch ON"); // Print message to serial
    sweepservo.write(120); // Move sweepservo to 120 degrees
    if (!collectorIsOn) { // If the dust collector is not on
      Serial.println("turnOnDustCollection"); // Print message to serial
      digitalWrite(dustCollectionRelayPin, LOW); // Turn on dust collection relay
      collectorIsOn = true; // Update collector state to on
    }
  } else { // If sweep switch is not pressed
    sweepservo.write(90); // Move sweepservo to 90 degrees
    if (collectorIsOn && sawswitchState != LOW) { // If collector is on and saw switch is not pressed
      Serial.println("Sweep Switch OFF"); // Print message to serial
      Serial.println("turnOffDustCollection"); // Print message to serial
      digitalWrite(dustCollectionRelayPin, HIGH); // Turn off dust collection relay
      collectorIsOn = false; // Update collector state to off
    }
  }

  if (sawswitchState == LOW) { // If saw switch is pressed
    Serial.println("Saw Switch ON"); // Print message to serial
    sawservo.write(120); // Move sawservo to 120 degrees
    digitalWrite(tablesawRelayPin, LOW); // Turn on table saw relay
    delay(15); // Wait for 15ms for servo to move
    if (!collectorIsOn) { // If the dust collector is not on
      Serial.println("turnOnDustCollection"); // Print message to serial
      digitalWrite(dustCollectionRelayPin, LOW); // Turn on dust collection relay
      collectorIsOn = true; // Update collector state to on
      digitalWrite(tablesawRelayPin, LOW); // Ensure table saw relay is on
      delay(15); // Wait for 15ms for relay to activate
    }
  } else { // If saw switch is not pressed
    sawservo.write(90); // Move sawservo to 90 degrees
    digitalWrite(tablesawRelayPin, HIGH); // Turn off table saw relay
    delay(15); // Wait for 15ms for servo to move
    if (collectorIsOn && sweepswitchState != LOW) { // If collector is on and sweep switch is not pressed
      Serial.println("Saw Switch OFF"); // Print message to serial
      Serial.println("turnOffDustCollection"); // Print message to serial
      digitalWrite(dustCollectionRelayPin, HIGH); // Turn off dust collection relay
      digitalWrite(tablesawRelayPin, HIGH); // Ensure table saw relay is off
      collectorIsOn = false; // Update collector state to off
      delay(15); // Wait for 15ms for relay to deactivate
    }
  }
}
