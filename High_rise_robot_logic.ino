#include <Servo.h>
#include <IRremote.h>

int collisionSensorArrayPins[] = {3, 2, 4, 5};
int collisionSensorPinCount = sizeof(collisionSensorArrayPins)/sizeof(collisionSensorArrayPins[0]);
int collisionSensorArrayTrigger = 7;

int motorASpeedPin = 9;
int motorAInputAPin = 8;
int motorAInputBPin = 10;

int motorBSpeedPin = 6;
int motorBInputAPin = 11;
int motorBInputBPin = 12;

int infraredSensorPin = A0;

int upperArmatureServoPin = A1;
int baseArmatureServoPin = A2;

int rightClawServoPin = A3;
int leftClawServoPin = A4;

Servo upperArmatureServo;
Servo baseArmatureServo;
Servo leftClawServo;
Servo rightClawServo;

bool collisionDetectionWarning = false;
bool ignoreCollisionWarning = false;

bool moveMode = true;

const float SOUND_SPEED = 0.0135; //inches per microsecond.

//Where inits are setup.
void setup() {
  //Set up ultrasonic-distance sensor array
  pinMode(collisionSensorArrayTrigger, OUTPUT);	
  for (int sensorNum = 0; sensorNum < collisionSensorPinCount; sensorNum++) {
   	pinMode(collisionSensorArrayPins[sensorNum], INPUT);
  }
  
  //setup motors
  initMotor(motorASpeedPin, motorAInputAPin, motorAInputBPin);
  initMotor(motorBSpeedPin, motorBInputAPin, motorBInputBPin);
  
  //servo pinout setup
  pinMode(upperArmatureServoPin, OUTPUT);
  pinMode(baseArmatureServoPin, OUTPUT);
  pinMode(rightClawServoPin, OUTPUT);
  pinMode(leftClawServoPin, OUTPUT);
  
  //attach pins to servo
  upperArmatureServo.attach(upperArmatureServoPin);
  baseArmatureServo.attach(baseArmatureServoPin);
  leftClawServo.attach(leftClawServoPin);
  rightClawServo.attach(rightClawServoPin);
  
  //begin serial output
  Serial.begin(9600);
  //ready to read infrared sensor with remote
  IrReceiver.begin(infraredSensorPin);
  
  //set upper arm and base to retracted
  upperArmatureServo.write(0);
  baseArmatureServo.write(0);
  
  //set claw to open
  openClaw();
  
}

//Init motor func.
void initMotor(int motorSpeedPin, int motorInputA, int motorInputB) {
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorInputA, OUTPUT);
  pinMode(motorInputB, OUTPUT);
}

//Main loop
void loop() {
  readCollisionDetection();
  
  int buttonPressed = readInfrared();
  if (buttonPressed >= 0) {
    switch(buttonPressed) {
      case 1:
      	closeClaw();
      	break;
      case 2: //Disable Collision Warn / (FUNC/STOP)
      	Serial.println("Ignoring warning message now...");
      	ignoreCollisionWarning = true;
      	break;
      case 4: //Go Left / Extend upper arm / (FAST-REVERSE??)
      	moveLeft();
      	break;
      case 6: //Go Right / Retract upper arm /(FAST-FORWARD)
      	moveRight();
      	break;
      case 8: //Reverse / Retract lower armature / (DOWN)
      	moveBackward();
      	break;
      case 9:
      	openClaw();
      	break;
      case 10: //Forward / extend lower armature / (UP)
      	moveForward();
      	break;
      case 13: //Change mode / (EQ)
      	Serial.println("Changing mode...");
      	if (moveMode)
          moveMode = false;
      	else
          moveMode = true;
      	break;
      default:
      	break;
    }
  }
  
  delay(100);
}

 //move left OR extend upper arm
void moveLeft() {
  if (moveMode) {
    if (!collisionDetectionWarning || ignoreCollisionWarning) {
      Serial.println("Moving Left!!!");
      //Set motors half speed
      analogWrite(motorASpeedPin, 127); //50%
      analogWrite(motorBSpeedPin, 127); //50%

      digitalWrite(motorAInputAPin, HIGH);
      digitalWrite(motorAInputBPin, LOW);
      
      
      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, HIGH);

      delay(500); //1/2 a second move

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, LOW);

      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, LOW);
    }
    
  } else {
    //extend armature upper arm
    upperArmatureServo.write(180);
    Serial.println("Extending upper arm!!!");
  }
}

//move right OR retract upper arm
void moveRight() {
  if (moveMode) {
    if (!collisionDetectionWarning || ignoreCollisionWarning) {
      Serial.println("Moving Right!!!");
      //Set motors half speed
      analogWrite(motorASpeedPin, 127); //50%
      analogWrite(motorBSpeedPin, 127); //50%

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, HIGH);

      digitalWrite(motorBInputAPin, HIGH);
      digitalWrite(motorBInputBPin, LOW);

      delay(500); //1/2 a second move

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, LOW);

      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, LOW);
    }
  } else {
   	//retract armature upper arm
    upperArmatureServo.write(0);
    Serial.println("Retracting upper arm!!!");
  }
}

//move forward OR extend arm base
void moveForward() {
  if (moveMode) {
    if (!collisionDetectionWarning || ignoreCollisionWarning) {
      Serial.println("Moving Forwards!!!");
      //Set motors full speed
      analogWrite(motorASpeedPin, 255); //100%
      analogWrite(motorBSpeedPin, 255); //100%

      digitalWrite(motorAInputAPin, HIGH);
      digitalWrite(motorAInputBPin, LOW);

      digitalWrite(motorBInputAPin, HIGH);
      digitalWrite(motorBInputBPin, LOW);

      delay(1000); //1 second move

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, LOW);

      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, LOW);
    }
  } else {
   	//extend armature base
    baseArmatureServo.write(0);
    Serial.println("Extending arm base!!!");
  }
}

//move backwards OR retract arm base
void moveBackward() {
  if (moveMode) {
    if (!collisionDetectionWarning || ignoreCollisionWarning) {
      Serial.println("Moving Backwards!!!");
      //Set motors full speed
      analogWrite(motorASpeedPin, 255); //100%
      analogWrite(motorBSpeedPin, 255); //100%

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, HIGH);

      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, HIGH);

      delay(1000); //1 second move

      digitalWrite(motorAInputAPin, LOW);
      digitalWrite(motorAInputBPin, LOW);

      digitalWrite(motorBInputAPin, LOW);
      digitalWrite(motorBInputBPin, LOW);
    }
  } else {
   	//retract armature base.
    baseArmatureServo.write(180);
    Serial.println("Retracting arm base!!!");
  }
}

//open using claw servos
void openClaw() {
  Serial.println("Claw --- Open");
  leftClawServo.write(180);
  rightClawServo.write(0);
}

//close claw using servos
void closeClaw() {
  Serial.println("Claw --- Closed");
  leftClawServo.write(0);
  rightClawServo.write(180);
}

//read usd sensors
void readCollisionDetection() {
  
  bool collisionForseen = false;
  
  //Check distance vals at each sensor for future collide.
  for (int sensorNum = 0; sensorNum < collisionSensorPinCount; sensorNum++) {
    if (isCollisionForseen(sensorNum)) {
      collisionForseen = true;
      break;
    }
  }
  
  //If sensors dont pick up anything, reset global values.
  //If sensors pick up, and we are ignoring warning, ignore global detect value.
  if (!collisionForseen) {
    collisionDetectionWarning = false;
    ignoreCollisionWarning = false;
  } else {
    if (ignoreCollisionWarning) {
      Serial.print("Collision Warning Ignored");
      Serial.println();
      collisionDetectionWarning = false;
    } else {
      collisionDetectionWarning = true;
    }
  }
  
  if (collisionDetectionWarning) {
   	Serial.print("Collision Detected");
  }
  Serial.println();
}

//check each sensor -- helper method
bool isCollisionForseen(int sensorNum) {
  
  digitalWrite(collisionSensorArrayTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(collisionSensorArrayTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(collisionSensorArrayTrigger, LOW);
  
  //If sensor detects at 3in and less, throw warning detect. (true)
  float distance = 0.5 * SOUND_SPEED * pulseIn(collisionSensorArrayPins[sensorNum], HIGH);
  delay(60);
  return (distance <= 3);
}









//helper methods for infrared sensor reading.

/////////////////////////////////
//Taken from TinkerCAD resources.
/////////////////////////////////

// Map the IR code to the corresponding remote button.
// The buttons are in this order on the remote:
//    0   1   2
//    4   5   6
//    8   9  10
//   12  13  14
//   16  17  18
//   20  21  22
//   24  25  26
//
// Return -1, if supplied code does not map to a key.
int mapCodeToButton(unsigned long code) {
  // For the remote used in the Tinkercad simulator,
  // the buttons are encoded such that the hex code
  // received is of the format: 0xiivvBF00
  // Where the vv is the button value, and ii is
  // the bit-inverse of vv.
  // For example, the power button is 0xFF00BF000

  // Check for codes from this specific remote
  if ((code & 0x0000FFFF) == 0x0000BF00) {
    // No longer need the lower 16 bits. Shift the code by 16
    // to make the rest easier.
    code >>= 16;
    // Check that the value and inverse bytes are complementary.
    if (((code >> 8) ^ (code & 0x00FF)) == 0x00FF) {
      return code & 0xFF;
    }
  }
  return -1;
}

int readInfrared() {
  int result = -1;
  // Check if we've received a new code
  if (IrReceiver.decode()) {
    // Get the infrared code
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    // Map it to a specific button on the remote
    result = mapCodeToButton(code);
    // Enable receiving of the next value
    IrReceiver.resume();
  }
  return result;
}
