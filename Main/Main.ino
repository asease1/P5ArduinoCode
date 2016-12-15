//Interrupts pin 2-3
#define interupt1 2
#define interupt2 3
//Dircation pins x axis
#define xPin1 5
#define xPin2 4
//Dircation pins y axis
#define yPin1 6
#define yPin2 7
//Diraction pins z axis
#define zPin1 13
#define zPin2 8
//Chanel Pins
#define chanelPin3 10
#define chanelPin2 11
#define chanelPin1 12


//gear are the amount of speed the motor should move at from 0-255
#define gear1 255
#define gear2 110
#define gear3 140

#define Hold_Delay 500
#define DELAY_FOR_MOTOR_MOVEMENT 600

#define MAX_QUEUE_SIZE 25

#include "Wire.h"
#include "Interface.h"



void setup() {
 
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

  pinMode(gearPin, OUTPUT);

  pinMode(interupt1, INPUT_PULLUP);
  pinMode(interupt2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interupt1), OnInterupts1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interupt2), OnInterupts2, CHANGE);


  Serial.begin(9600);
  queue = CreateQueue(sizeof(Instruction));

  BPProgress.x = 0;
  BPProgress.y = 0;
  BPProgress.z = 0;

  wallQueue = CreateQueue(sizeof(Wall));
  
  myController = CreateController(CreateMotor(3000, xPin1, xPin2),CreateMotor(3050, yPin1, yPin2),CreateMotor(3050, zPin1, zPin2));


  
  
  ResetSystem();

  Serial.print("Input: 5 ");
  Serial.print("Output: ");
  Serial.println(ConvertToGearDegrees(5));
  Serial.print("Input: -5 ");
  Serial.print("Output: ");
  Serial.println(ConvertToGearDegrees(-5));
  Serial.print("Input: 100000 ");
  Serial.print("Output: ");
  Serial.println(ConvertToGearDegrees(100000));
  Serial.print("Input: 0 ");
  Serial.print("Output: ");
  Serial.println(ConvertToGearDegrees(0));
  Serial.print("Input: 35 ");
  Serial.print("Output: ");
  Serial.println(ConvertToGearDegrees(35));

}

void loop() {

}

//DebugCode



void OnInterupts1(){
  //Serial.println(11);
  //NXT lego motor encoder
  //A1100
  //B0110 Code for the clockwise rotation
  int pinSignal = digitalRead(interupt1);

  if(myController.runningMotor->sig1 == pinSignal)
    return;
  
  myController.runningMotor->sig1 = pinSignal;
  switch(pinSignal){
    case 0:
      if(myController.runningMotor->sig2 == 0){
        myController.runningMotor->pos += -1;
      }
      else{
        myController.runningMotor->pos += 1;
      }
    case 1:
      if(myController.runningMotor->sig2 == 0){
        myController.runningMotor->pos += 1;
      }
      else{
        myController.runningMotor->pos += -1;
      }
      break;
  }  
  OnInterrupt();
  //Serial.println(21);
}

void OnInterupts2(){
  //Serial.println(12);
  //NXT lego motor encoder
  //A1100
  //B0110 Code for the clockwise rotation
  int pinSignal = digitalRead(interupt2);

  //Check if the signal is the same as before
  //if it is then we have changed chanel and should not run the interupt
  if(myController.runningMotor->sig2 == pinSignal)
     return;
      
  //Save the change the sig to the new bit
  myController.runningMotor->sig2 = pinSignal;

  switch(pinSignal){
    case 0:
      if(myController.runningMotor->sig1 == 0){
        myController.runningMotor->pos += 1;
      }
      else{
        myController.runningMotor->pos += -1;
      }
    case 1:
      if(myController.runningMotor->sig1 == 0){
        myController.runningMotor->pos += -1;
      }
      else{
        myController.runningMotor->pos += 1;
      }
      break;
  }

   OnInterrupt();
   //Serial.println(22);
}

bool InterruptMotorPositionCheck(){
    bool halted = false;
    
    switch(myController.runningMotor->state){
      case forward:
        if(myController.runningMotor->pos >= currentInstruction->positions[currentInstruction->count] - ERROR_MARGIN1){
          //Reset the speed of the motor
          analogWrite(gearPin, gear1);  
          ChangeMotorState(hold, myController.runningMotor);
          
          halted = true;
        }
        else if(myController.runningMotor->pos >= currentInstruction->positions[currentInstruction->count] - ERROR_MARGIN2){
          if(myController.runningMotor == &myController.motorZ)
            analogWrite(gearPin, gear2-30);
          else
            analogWrite(gearPin, gear2);
          
        }
        else if(myController.runningMotor->pos >= currentInstruction->positions[currentInstruction->count] - ERROR_MARGIN3){
          analogWrite(gearPin, gear3);
        }
        break;
      case backward:
        if(myController.runningMotor->pos <= currentInstruction->positions[currentInstruction->count] + ERROR_MARGIN1){
          //Reset the speed of the motor
          analogWrite(gearPin, gear1);
          ChangeMotorState(hold, myController.runningMotor);
          
          halted = true;
        }
        else if(myController.runningMotor->pos <= currentInstruction->positions[currentInstruction->count] + ERROR_MARGIN2){
          if(myController.runningMotor == &myController.motorZ)
            analogWrite(gearPin, gear2-30);
          else
            analogWrite(gearPin, gear2);
          
        }
        else if(myController.runningMotor->pos <= currentInstruction->positions[currentInstruction->count] + ERROR_MARGIN3){
          analogWrite(gearPin, gear3);
        }
        break;
      case hold:
        break;
    }
    
      //Serial.println(halted);
    return halted;
}

void OnInterrupt(){
  TimeSinceLastInterrupt = millis();
  
  if(isResat && InterruptMotorPositionCheck()){
    isPosReached = true;
  }
}



void ResetSystem(){
  isResat = false;
  analogWrite(gearPin, 180);
  ResetMotor(motorY);
  analogWrite(gearPin, 140);
  ResetMotor(motorZ);
  analogWrite(gearPin, 140);
  ResetMotor(motorX); 
  isResat = true;
  analogWrite(gearPin, 255);
}

bool CheckPositionMargen(){
  if(myController.runningMotor->pos <= currentInstruction->positions[currentInstruction->count] + ERROR_MARGIN1 && myController.runningMotor->pos >= currentInstruction->positions[currentInstruction->count] - ERROR_MARGIN1)
    return true;
  return false; 
}

void ResetMotor(Chanels motor){
  SelectMotor(&myController, motor);
  ChangeMotorState(backward, myController.runningMotor);
  TimeSinceLastInterrupt = millis();
  while(IsCurrentMotorMoving()){
    //Serial.println(myController.runningMotor->pos);
  }
  ChangeMotorState(hold, myController.runningMotor);
  myController.runningMotor->pos = 0;
}





