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

struct Instruction{
  int positions[4];
  short int count;
};

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
