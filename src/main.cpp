#include <Arduino.h>
#include <TeensyThreads.h>

// Output PINs
const int redLedPin = 13;
const int orangeLedPin = 16;
const int greenLedPin = 19;
const int buzzerPin = 22;

// Reset Time
const int resetAfter = 30000;

// Possible states
enum States { NONE=0, ALARM=1, ALERT=2, ERR=3, CAUTION=5, WARN=7, OKAY=9, INIT=10 };

// State of the system and serialData
volatile States state = INIT;
volatile int serialData = 0;
volatile unsigned long lastDataReceived = millis();

void ledThread() {
  while(1) {
    States tempState = state;
    // Run for each state
    switch(state) {
      case ALARM:
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        
        digitalWrite(redLedPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        threads.delay(200);
        digitalWrite(buzzerPin, LOW);
        threads.delay(200);
        digitalWrite(buzzerPin, HIGH);
        threads.delay(200);
        digitalWrite(redLedPin, LOW);
        digitalWrite(buzzerPin, LOW);
        if (tempState == state) {
          state = ALERT;
        }
        threads.delay(600);
        break;

      case ALERT:
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);
        
        digitalWrite(redLedPin, HIGH);
        threads.delay(600);
        digitalWrite(redLedPin, LOW);
        threads.delay(600);
        break;

      case ERR:
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);

        digitalWrite(redLedPin, HIGH);
        threads.delay(1000);
        break;

      case CAUTION:
        digitalWrite(redLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);
        
        digitalWrite(orangeLedPin, HIGH);
        threads.delay(600);
        digitalWrite(orangeLedPin, LOW);
        threads.delay(600);
        break;

      case WARN:
        digitalWrite(redLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);

        digitalWrite(orangeLedPin, HIGH);
        threads.delay(1000);
        break;

      case OKAY:
        digitalWrite(redLedPin, LOW);
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(buzzerPin, LOW);

        digitalWrite(greenLedPin, HIGH);
        threads.delay(1000);
        break;

      case NONE:
        digitalWrite(redLedPin, LOW);
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);
        threads.delay(1000);
        break;

      case INIT:
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);

        digitalWrite(redLedPin, HIGH);
        threads.delay(600);
        digitalWrite(redLedPin, LOW);
        digitalWrite(orangeLedPin, HIGH);
        threads.delay(600);
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, HIGH);
        threads.delay(600);
        break;

      default:
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
        digitalWrite(buzzerPin, LOW);

        digitalWrite(redLedPin, HIGH);
        threads.delay(1000);
        digitalWrite(redLedPin, LOW);
        threads.delay(1000);
        break;
    }
  }
}

void setup() {
  // Set Pins as output pins
  pinMode(redLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Open serial connection
  Serial.begin(9600);

  // Start LED control thread
  threads.addThread(ledThread);
}

void loop() {
  // Read serial data
  serialData = Serial.read();

  // Update state if a single-digit number has been sent
  if (serialData >= '0' && serialData <= '9' ) {
    // Convert from character to number
    state = (States)(serialData - '0');
    // Set last data received time
    lastDataReceived = millis();
  }

  // Reset if no data has been delivered since a certain time
  if (abs(millis() - lastDataReceived) > resetAfter) {
    state = INIT;
  }

  delay(200);
}
