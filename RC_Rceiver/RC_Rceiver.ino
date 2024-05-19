#include<Wire.h>
#include <SoftwareSerial.h>

// SoftwareSerial object for communication with receiver Arduino
SoftwareSerial mySerial(0, 1); // RX, TX pins for communication

// Motor Variables
const int IN1 = 13;
const int IN2 = 12;
const int IN3 = 7;
const int IN4 = 8;
const int ENA = 11;
const int ENB = 6;

// Linear velocity, Rotation & Motor Speed
float x_dot = 0;
float theta_dot = 0;
float phi1, phi2, pwm1, pwm2; // Motor Speed 

void setup() {
  // Initialize serial communication with receiver Arduino
  mySerial.begin(38400);

  // Pins Initialization
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

}

void loop() {
  // Read data string from transmitter Arduino

  if (mySerial.available()){
    String cmd = mySerial.readStringUntil('\n');

      // Split data string into individual values    
      sscanf(cmd.c_str(), "%f,%f", &x_dot, &theta_dot);
  }

  /*
  Forward  :: x_dot >> 1 , theta_dot >> 0
  Backward :: x_dot >> -1 , theta_dot >> 0

  Left  :: x_dot >> 0 , theta_dot >> -1
  Right :: x_dot >> 0 , theta_dot >> 1

  FR , FL BR, BL :: x_dot >> Variable , theta_dot >> Variable 
  */

  phi1 = x_dot - theta_dot;
  phi2 = x_dot + theta_dot; 

  // Set Motor Speed To The Range of [-1, 1] >> got from range of required angles
  pwm1 = floatMap(phi1, -1, 1, -255, 255);
  pwm2 = floatMap(phi2, -1, 1, -255, 255);

  moveWheels(pwm1, pwm2);
  
}


float floatMap(float input, float inputMin, float inputMax, float outputMin, float outputMax) {
  // Ensure the input value is within the input range
  input = constrain(input, inputMin, inputMax);

  // Calculate the input range and output range
  float inputRange = inputMax - inputMin;
  float outputRange = outputMax - outputMin;

  // Map the input value to the output range
  float result = ((input - inputMin) / inputRange) * outputRange + outputMin;
  int output = int(result);
  return output;
}

void moveWheels(int pwm1, int pwm2){
  // ============== SETTING DIRECTION ONLY ================
  // WHEEL 1
  if (pwm1 < 0){

    // Set wheel 1 in BACWARD direction
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else{

    // Set wheel 1 in FORWARD direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  // WHEEL 2
  if (pwm2 < 0){

    // Set wheel 2 in BACWARD direction
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else{

    // Set wheel 2 in FORWARD direction
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // ============== SETTING SPEED ONLY ================
  int Speed1 = abs(pwm1);
  int Speed2 = abs(pwm2);

  // Limit PWM to maximum 255
  if (Speed1 > 255){
    Speed1 = 255;
  }
  if (Speed2 > 255){
    Speed2 = 255;
  }

  // Write SPEEDS to motors
  analogWrite(ENA, Speed1);
  analogWrite(ENB, Speed2);
}
