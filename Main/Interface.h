//The functions in Interface.h are the ones that control the motors, and in general they act as an interface for the program to interact with the real world
//It defines motors and a controller that contains the motors, that the other parts of the program can access
//Has a struct for position/3DVector and a brickData struckt
//Margins, used for correcting motor movement
#define ERROR_MARGIN1 6
#define ERROR_MARGIN2 120
#define ERROR_MARGIN3 600

//Coordinates for where the robot has to pick up bricks
#define SMALL_BRICK_DEPO_X 0
#define SMALL_BRICK_DEPO_Z 7
#define LARGE_BRICK_0_DEPO_X 0
#define LARGE_BRICK_0_DEPO_Z 0
#define LARGE_BRICK_90_DEPO_X 0
#define LARGE_BRICK_90_DEPO_Z 14

//Gearing pin
//Pin to the PVM pins(3,5,6,9,10,11)
#define gearPin 9

enum Channels {motorY, motorX, motorZ, motorRotation}; //Channel is used to change what motor is currently in use
enum MotorStates {forward, backward, hold}; //States of a motor
enum BrickType {smallBrick, largeBrick0, largeBrick90, none}; //The different types of brick we }
enum InstructionProgress {XCoord, ZCoord, NextInstruction}; // Tells at what stage an instruction is: Is it executing the X-coordinate, the z-coordinate, or if it should move on to the next instruction.
#include "Model.h"

unsigned long int TimeSinceLastInterrupt = 0;

bool isPosReached = false;
bool isResat = false;
bool newInstruction = false;
bool queueIsEmpty = true;

struct Instruction* currentInstruction;
struct Instruction* savedInstruction;



struct Motor{
    volatile int pos;
    int maxPos;
    int minPos;
    int pin1;
    int pin2;
    MotorStates state;
    volatile int sig1;
    volatile int sig2;
};

struct Controller{
  int targetPos;
  Motor motorX;
  Motor motorY;
  Motor motorZ;
  volatile Motor *runningMotor;
};

struct Controller myController;

Controller CreateController(Motor motorX, Motor motorY, Motor motorZ){
  Controller newController;
  newController.targetPos = 0;
  newController.motorX = motorX;
  newController.motorY = motorY;
  newController.motorZ = motorZ;
  newController.runningMotor = &motorX;
  return newController;
}



//Function that initializes a new motor
Motor CreateMotor(int maxPos, float pin1, float pin2){
  //maybe move pin setyp in to this function.
  Motor newMotor;
  newMotor.pos = 0;
  newMotor.maxPos = maxPos;
  newMotor.minPos = 0;
  newMotor.pin1 = pin1;
  newMotor.pin2 = pin2;
  newMotor.state = hold;
  //Can check the sig pins in here, if you change channel to get a more precise position.
  newMotor.sig1 = 0;
  newMotor.sig2 = 0;

  return newMotor;
}

//Takes a motor and sets the pins to the direction specified and changes the state accordingly.
void ChangeMotorState(MotorStates state, volatile Motor* motor){
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
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, LOW);
      motor->state = hold;
      break;
  }
}

//Update the controllers target position to a new position, and start the specified motor in that direction
bool MoveTo(int pos, volatile Controller *control){
  if(pos > control->runningMotor->maxPos)
    return false;

  
  if(control->runningMotor->pos > (pos + ERROR_MARGIN1)){
      ChangeMotorState(backward, control->runningMotor);
  }
  else if(control->runningMotor->pos < (pos - ERROR_MARGIN1)){
    ChangeMotorState(forward, control->runningMotor);
  }
  else
    return false;
  control->targetPos = pos;
  return true;
}



//Switch the runningMotor contained in the controller to a new motor
void SelectMotor(volatile Controller *myControl, Channels newMotor){
  switch(newMotor){
    case motorX:
      digitalWrite(ChannelPin1, HIGH);
      digitalWrite(ChannelPin2, LOW);
      digitalWrite(ChannelPin3, LOW);
      myControl->runningMotor = &myControl->motorX;
      break;
    case motorY:
      digitalWrite(ChannelPin1, LOW);
      digitalWrite(ChannelPin2, HIGH);
      digitalWrite(ChannelPin3, LOW);
      myControl->runningMotor = &myControl->motorY;
      break;
    case motorZ:
      digitalWrite(ChannelPin1, LOW);
      digitalWrite(ChannelPin2, LOW);
      digitalWrite(ChannelPin3, LOW);
      myControl->runningMotor = &myControl->motorZ;
      break;
	default:
		break;
  }
}

//Creates an instruction for where to pick up a brick. This instruction is passed on and executed somewhere else.
Instruction* PickUpBrick(BrickType brick){
  Instruction* pickUpInstruction = malloc(sizeof(Instruction));
  Instruction tempInstruction;
  Serial.print("okey so brick is: ");
  Serial.println(brick);
  switch(brick){
    case smallBrick:
      tempInstruction = CreateInstruction(SMALL_BRICK_DEPO_X,SMALL_BRICK_DEPO_Z,0, none);
      break;
    case largeBrick0:
      tempInstruction = CreateInstruction(LARGE_BRICK_0_DEPO_X, LARGE_BRICK_0_DEPO_Z,0, none);
      break;
    case largeBrick90:
      tempInstruction = CreateInstruction(LARGE_BRICK_90_DEPO_X, LARGE_BRICK_90_DEPO_Z,0, none);
      break;
  }

  tempInstruction.type = pickUp; 
  memcpy(pickUpInstruction, &tempInstruction, sizeof(Instruction));
  return pickUpInstruction;
}


bool IsCurrentMotorMoving(){
  if(millis() > TimeSinceLastInterrupt + DELAY_FOR_MOTOR_MOVEMENT){
    return false;
  }
  else{
    return true;
  }
}


//Executes the grabbing of a brick. Seperate from other kinds of motor movement. 
void GrabBrick(Controller *myController){
  ChangeMotorState(forward, &myController->motorY);
  delay(2000);
  analogWrite(gearPin, 180);
  isResat = false;
  Serial.println(isResat);
  SelectMotor(myController, motorZ);
  for(int i = 0; i < 0; i++){
    ChangeMotorState(forward, myController->runningMotor);
    delay(200);
    ChangeMotorState(backward, myController->runningMotor);
    delay(400);
    ChangeMotorState(forward, myController->runningMotor);
    delay(200);
    
    ChangeMotorState(hold, myController->runningMotor);
    delay(700);  
  }
  
  SelectMotor(myController, motorX);
  isResat = true;

  ChangeMotorState(backward, &myController->motorY);
  delay(2000);
  ChangeMotorState(hold, &myController->motorY);
}

//Executes the grabbing of a brick. Seperate from other kinds of motor movement. 
void PlaceBrick(volatile Controller *myController){

  ChangeMotorState(forward, &myController->motorY);
  delay(2000);
  analogWrite(gearPin, 180);
  isResat = false;
  Serial.println(isResat);
  SelectMotor(myController, motorX);
  for(int i = 0; i < 1; i++){
    ChangeMotorState(forward, &myController->motorX);
    delay(400);
    ChangeMotorState(backward, &myController->motorX);
    delay(800);
    ChangeMotorState(forward, &myController->motorX);
    delay(400);
    
    ChangeMotorState(hold, &myController->motorX);
    delay(700);  
  }
  
  SelectMotor(myController, motorZ);
  isResat = true;
  analogWrite(gearPin, 255);
  ChangeMotorState(backward, &myController->motorY);
  delay(2000);
  ChangeMotorState(hold, &myController->motorY);
}

void ExecuteInstruction(){
  SelectMotor(&myController, motorX);
  free(currentInstruction);

  if(queue.size == 0)
    queueIsEmpty = true;
  else
    queueIsEmpty = false;
  currentInstruction = (Instruction*)GetNextInstruction();
}

void StartMotor(){
  
  while(!MoveTo(currentInstruction->positions[currentInstruction->count],&myController)){

      if(newInstruction){
        newInstruction = false;
        isPosReached = true;
        return;
      }
      else if((currentInstruction->count+1) == NextInstruction){        
           currentInstruction->count = ZCoord;
           isPosReached = true;
           return;
      }
      else{
        switch(currentInstruction->count){
          case XCoord:
            SelectMotor(&myController, motorX);
            break;
          case ZCoord:
            SelectMotor(&myController, motorZ);
            break;
        }
      }
    }
}

//The loop responsible for running the right parts of an instruction when it is supposed to run, eg. the x coordinate first, then the z coordinate
void InterfaceLoop(){
if(progress == InstructionsCreated){
    ExecuteInstruction();
    isPosReached = true;
    progress = ReadyToRun;
  }
  else if(progress == ReadyToRun){
    if(isPosReached && !IsCurrentMotorMoving()){
    isPosReached = false;
    
      switch(currentInstruction->count){
        case XCoord:   
          if(currentInstruction->type == normalInst){
            
            savedInstruction = currentInstruction;
            currentInstruction = PickUpBrick(currentInstruction->brick);
            newInstruction = true;
            break;
          }
          currentInstruction->count = ZCoord;
          SelectMotor(&myController, motorZ);
          
          break;
        case ZCoord:
          if(currentInstruction->type == pickUp){
            GrabBrick(&myController);
            free(currentInstruction);
            currentInstruction = savedInstruction;
            currentInstruction->type = place;
            SelectMotor(&myController, motorX);
            break;
          }
            digitalWrite(gearPin, HIGH);
          PlaceBrick(&myController);
          ExecuteInstruction();
          
          break;
      }
      if(!queueIsEmpty){
        StartMotor();
      }
    }
  }
}



