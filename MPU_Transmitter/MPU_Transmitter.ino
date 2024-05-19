///////////////////////////////////////// TRASMITTER /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#include<Wire.h>
#include <SoftwareSerial.h>

// SoftwareSerial object for communication with receiver Arduino
SoftwareSerial mySerial(0, 1); // RX, TX pins for communication

// Define MPU address & 6 Axis
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

int minVal=265;
int maxVal=402;

// Define Angles  (set the required angle)
double x;
double y;
double z;

// Linear Velocity & Directio of Rotation 
float x_dot = 0;      // Forward & Backward Motion
float theta_dot = 0;  // Left & Right Motion

void setup() {
  // Initialize MPU
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(38400);
}



void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);

  // Read & Save Values (Accelerometer)
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();

  // Mapping values to range of degrees
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
  // Final Output angles
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  
  // Activate a deadzone for Angle x (+- 5 degrees) => Safety Condition 
  if(x > 175 and x < 185){
    x_dot = 0;

  }
  else{
    x_dot = floatMap(x, 90, 270, -1, 1); // MPU rotation around Y-axis (F & B)
  }

  // Activate a deadzone for Angle Y (+- 5 degrees) => Safety Condition 
  if (y > 175 and y < 185){
    theta_dot = 0;

  } else{
    theta_dot = floatMap(y, 90, 270, -1, 1); // MPU rotation around X-axis (L & R)
  }

  // Prepare data string to send to receiver Arduino
  String cmd = String(x_dot) + "," + String(theta_dot);

  // Send data string over serial communication
  mySerial.println(cmd);

  Serial.print("X_dot= ");
  Serial.println(x_dot);
  
  Serial.print("Theta_dot= ");
  Serial.println(theta_dot);

  Serial.println("-----------------------------------------");
  delay(400);
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
