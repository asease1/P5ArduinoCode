//Interrupts pin 2-3
#define interupt1 2
#define interupt2 3
//Dircation pins x axis
#define xPin1 4
#define xPin2 5
//Dircation pins z axis
#define zPin1 6
#define zPin2 7
//Diraction pins y axis
#define yPin1 9
#define yPin2 8
//Chanel Pins
#define chanelPin3 10
#define chanelPin2 11
#define chanelPin1 12
//Margins
#define ERROR_MARGIN 2
#define Hold_Delay 8000

#include "Wire.h";
#include "queue.h";

//Chanel is the current motor input Interupts
enum Chanels {motorY, motorX, motorZ, motorRotation};
enum motorstates {forward, backward, hold};

struct Motor{
    volatile int pos;
    int maxPos;
    int minPos;
    int pin1;
    int pin2;
    motorstates state;
    volatile int sig1;
    volatile int sig2;
};

struct Instruction{
  int positions[4];
  short int count;
};





int targetPos = 0;

struct Motor motor1;
struct Motor motor2;
struct Motor motor3;
struct Motor *runningMotor;

struct Instruction currentInstruction;

void setup() {

   Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
 Wire.beginTransmission(0x20);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
Wire.beginTransmission(0x20);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();
 
  // define pin mode for chanels
  pinMode(chanelPin3, OUTPUT);
  pinMode(chanelPin2, OUTPUT);
  pinMode(chanelPin1, OUTPUT);

  //Y bricks 4 Deegres 4*10*15=600
  motor1 = {0, 1050, 0, yPin2, yPin1, hold,0,0};
  //X bricks 10 Deegres 10*10*15-(size platform 30*15)=1050
  motor2 = {0, 1050, 0, xPin2, xPin1, hold,0,0};
  //Z bricks 6 Deegres 6*10*15-(20*15)= 600
  motor3 = {0, 600, 0, zPin1, zPin2, hold, 0,0};
  
  pinMode(xPin1, OUTPUT);
  pinMode(xPin2, OUTPUT);
  pinMode(yPin1, OUTPUT);
  pinMode(yPin2, OUTPUT);
  pinMode(zPin1, OUTPUT);
  pinMode(zPin2, OUTPUT);

  pinMode(interupt1, INPUT_PULLUP);
  pinMode(interupt2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interupt1), OnInterupts1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interupt2), OnInterupts2, CHANGE);

  Serial.begin(9600);

  currentInstruction = CreateInstruction(0, 200, 300, 265);

  ChangeMotor(motorZ);
  MoveTo(currentInstruction.positions[currentInstruction.count], runningMotor);
}

void loop() {
  // put your main code here, to run repeatedly:

  



  
  Serial.println(runningMotor->pos);
}

Instruction CreateInstruction(int rotation, int x, int y, int z){
  Instruction newInstruction;
  newInstruction.positions[0] = rotation;
  newInstruction.positions[1] = x;
  newInstruction.positions[2] = y;
  newInstruction.positions[3] = z;
 
  newInstruction.count = 3; //Change to 0 when doing rotation :3

  return newInstruction;
}

//Dukomotion for the diffrent Error_Codes
//
void ErrorCode(int error_Code){
  Wire.beginTransmission(0x20); //Ip of the chip we want to talk to
  Wire.write(0x12); // choice the side of the chip
  //side A 0x13 and B 0x13 
  Wire.write(error_Code); //turn on the pin with a number from 0-255
  Wire.endTransmission();
}

void MoveTo(int pos, Motor* motor){
  //Maybe optimize here, save motor state on motor.

  if(motor->pos > (pos + ERROR_MARGIN)){
      ChangeMotorState(backward, motor);
  }
  else if(motor->pos < (pos - ERROR_MARGIN)){
    ChangeMotorState(forward, motor);
  }

  targetPos = pos;
}

void OnInterupts1(){
  //NXT lego motor encoder
  //A1100
  //B0110 Code for the clockwise rotation
  int pinSignal = digitalRead(interupt1);

  if(runningMotor->sig1 == pinSignal)
    return;
  
  runningMotor->sig1 = pinSignal;
  switch(pinSignal){
    case 0:
      if(runningMotor->sig2 == 0){
        runningMotor->pos += -1;
      }
      else{
        runningMotor->pos += 1;
      }
    case 1:
      if(runningMotor->sig2 == 0){
        runningMotor->pos += 1;
      }
      else{
        runningMotor->pos += -1;
      }
      break;
  }  
  OnInterrupt();
}

void OnInterupts2(){
  //NXT lego motor encoder
  //A1100
  //B0110 Code for the clockwise rotation
  int pinSignal = digitalRead(interupt2);

  //Check if the signal is the same as before
  //if it is then we have changed chanel and should not run the interupt
  //if(runningMotor->sig2 == pinSignal)
  //   return;
      
  //Save the change the sig to the new bit
  runningMotor->sig2 = pinSignal;

  switch(pinSignal){
    case 0:
      if(runningMotor->sig1 == 0){
        runningMotor->pos += 1;
      }
      else{
        runningMotor->pos += -1;
      }
    case 1:
      if(runningMotor->sig1 == 0){
        runningMotor->pos += -1;
      }
      else{
        runningMotor->pos += 1;
      }
      break;
  }

   OnInterrupt();
}

bool InterruptMotorPositionCheck(){
    bool halted = false;
    
    switch(runningMotor->state){
      case forward:
        if(runningMotor->pos >= currentInstruction.positions[currentInstruction.count] - ERROR_MARGIN){
          ChangeMotorState(hold, runningMotor);
          halted = true;
        }
        break;
      case backward:
        if(runningMotor->pos <= currentInstruction.positions[currentInstruction.count] + ERROR_MARGIN){
          ChangeMotorState(hold, runningMotor);
          halted = true;
        }
        break;
      case hold:
        break;
    }

    return halted;
}

void OnInterrupt(){
  if(InterruptMotorPositionCheck()){
    switch(currentInstruction.count){
      case 0:
        currentInstruction.count = 1;
        ChangeMotor(motorX);
        break;
      case 1:
        currentInstruction.count = 2;
        ChangeMotor(motorY);
        break;
      case 2:
        currentInstruction.count = 3;
        ChangeMotor(motorZ);
        break;
      case 3:
        currentInstruction.positions[3] = 0;
        break;
    }
    MoveTo(currentInstruction.positions[currentInstruction.count], runningMotor);
  }
}

void ChangeMotorState(motorstates state, Motor* motor){
  switch(state){
    case forward:
      motor->state = state;
      digitalWrite(motor->pin1, HIGH);
      digitalWrite(motor->pin2, LOW);
      break;
    case backward:
      motor->state = state;
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, HIGH);
      break;
    case hold:
      if(motor->state == forward)
        ChangeMotorState(backward, motor);
      else if(motor->state == backward)  
        ChangeMotorState(forward, motor);
      delay(Hold_Delay);
    
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, LOW);
      motor->state = hold;

      //Debug
      //MoveTo(motor->pos * -1, runningMotor);
      break;
  }
}

void ChangeMotor(Chanels newMotor){
  switch(newMotor){
    case motorX:
      digitalWrite(chanelPin1, HIGH);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      runningMotor = &motor2;
      break;
    case motorY:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      runningMotor = &motor1;
      break;
    case motorZ:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, HIGH);
      digitalWrite(chanelPin3, LOW);
      runningMotor = &motor3;
      break;
     //Make case for rotation
  }
}
