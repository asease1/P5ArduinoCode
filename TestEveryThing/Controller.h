//Control that defines motor and a controler that contains the motors
//Have function that contains the controller or motor in them
//Have struct for position/3DVector and a brickData struckt
//Margins
#define ERROR_MARGIN1 6
#define ERROR_MARGIN2 120
#define ERROR_MARGIN3 600

#define SMALL_BRICK_DEPO_X 0
#define SMALL_BRICK_DEPO_Z 7
#define LARGE_BRICK_0_DEPO_X 0
#define LARGE_BRICK_0_DEPO_Z 0
#define LARGE_BRICK_90_DEPO_X 0
#define LARGE_BRICK_90_DEPO_Z 14

//Gear Pin
//Pin to the PVM pins(3,5,6,9,10,11)
#define gearPin 9
//Chanel is the current motor input Interupts
enum Chanels {motorY, motorX, motorZ, motorRotation};
enum MotorStates {forward, backward, hold};
enum BrickType {smallBrick, largeBrick0, largeBrick90, none};
#include "Model.h"

unsigned long int TimeSinceLastInterrupt = 0;

bool isPosReached = false;
bool isResat = false;
bool newInstruction = false;
bool queueIsEmpty = true;

struct Instruction* currentInstruction;
struct Instruction* savedInstruction;



//All data struct
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

//Counstructers for all the struct
Controller CreateController(Motor motorX, Motor motorY, Motor motorZ){
  Controller newController;
  newController.targetPos = 0;
  newController.motorX = motorX;
  newController.motorY = motorY;
  newController.motorZ = motorZ;
  newController.runningMotor = &motorX;
  return newController;
}



//Function that create the motor with the values that a dynamic at the start
Motor CreateMotor(int maxPos, float pin1, float pin2){
  //maybe move pin setyp in to this function.
  Motor newMotor;
  newMotor.pos = 0;
  newMotor.maxPos = maxPos;
  newMotor.minPos = 0;
  newMotor.pin1 = pin1;
  newMotor.pin2 = pin2;
  newMotor.state = hold;
  //Can check the sig pins in here if you chance chanel to get a more precise position.
  newMotor.sig1 = 0;
  newMotor.sig2 = 0;

  return newMotor;
}

//Take a motor and set the pins to the diraction you want it to move and chance the state
//Have to watch out for that you can run a motor that the channel is not on if this is don't you cannot update its position.
void ChangeMotorState(MotorStates state, volatile Motor* motor){
  switch(state){
    case forward:
      motor->state = state;
      
      //Serial.println(motor->pin1);
      //Serial.println(motor->pin2);
      
      digitalWrite(motor->pin1, HIGH);
      digitalWrite(motor->pin2, LOW);
      break;
    case backward:
      motor->state = state;
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, HIGH);
      break;
    case hold:
      /*if(motor->state == forward)
        ChangeMotorState(backward, motor);
      else if(motor->state == backward)  
        ChangeMotorState(forward, motor);
      //we should find a other way, maybe use PVM and gears so we slow down.
      delay(Hold_Delay);*/
    
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, LOW);
      motor->state = hold;

      //Debug
      //MoveTo(motor->pos * -1, runningMotor);
      break;
  }
}

//Update the controller targetpos to the new position and start the runningMotor in the dircation of target
bool MoveTo(int pos, volatile Controller *control){
  if(pos > control->runningMotor->maxPos)
    return false;

  
  if(control->runningMotor->pos > (pos + ERROR_MARGIN1)){
      ChangeMotorState(backward, control->runningMotor);
      //Serial.println("Back");
  }
  else if(control->runningMotor->pos < (pos - ERROR_MARGIN1)){
    ChangeMotorState(forward, control->runningMotor);
    //Serial.println("Forward");
  }
  else
    return false;

  control->targetPos = pos;
  return true;
}



//Swi'tch the runningMotor on the Controller to a new Motor
void ChangeMotor(volatile Controller *myControl, Chanels newMotor){
  switch(newMotor){
    case motorX:
      digitalWrite(chanelPin1, HIGH);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      myControl->runningMotor = &myControl->motorX;
      break;
    case motorY:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, HIGH);
      digitalWrite(chanelPin3, LOW);
      myControl->runningMotor = &myControl->motorY;
      break;
    case motorZ:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      myControl->runningMotor = &myControl->motorZ;
      break;
    case motorRotation:
      //place code for rotationMotor.
      break;
  }
}


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
  //Serial.println(TimeSinceLastInterrupt);
  if(millis() > TimeSinceLastInterrupt + DELAY_FOR_MOTOR_MOVEMENT){
    return false;
  }
  else{
    //Serial.println(myController.runningMotor->pos);
    return true;
  }
}

void GrabBrick(Controller *myController){
  ChangeMotorState(forward, &myController->motorY);
  delay(2000);
  analogWrite(gearPin, 180);
  isResat = false;
  Serial.println(isResat);
  ChangeMotor(myController, motorZ);
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
  
  ChangeMotor(myController, motorX);
  isResat = true;

  ChangeMotorState(backward, &myController->motorY);
  delay(2000);
  ChangeMotorState(hold, &myController->motorY);
}

void PlaceBrick(volatile Controller *myController){

  ChangeMotorState(forward, &myController->motorY);
  delay(2000);
  analogWrite(gearPin, 180);
  isResat = false;
  Serial.println(isResat);
  ChangeMotor(myController, motorX);
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
  
  ChangeMotor(myController, motorZ);
  isResat = true;
  analogWrite(gearPin, 255);
  ChangeMotorState(backward, &myController->motorY);
  delay(2000);
  ChangeMotorState(hold, &myController->motorY);
}

void NextInstruction(){
  ChangeMotor(&myController, motorX);
  free(currentInstruction);

  if(queue.size == 0)
    queueIsEmpty = true;
  else
    queueIsEmpty = false;
  currentInstruction = (Instruction*)pop(&queue);
}

void StartMotor(){
  //Serial.println(currentInstruction->positions[currentInstruction->count]);
  
  while(!MoveTo(currentInstruction->positions[currentInstruction->count],&myController)){

      if(newInstruction){
        newInstruction = false;
        isPosReached = true;
        return;
      }
      else if(++currentInstruction->count == 2){        
           currentInstruction->count = 1;
           isPosReached = true;
           return;
        //}
      }
      else{
        switch(currentInstruction->count){
          case 0:
            ChangeMotor(&myController, motorX);
            break;
          case 1:
            ChangeMotor(&myController, motorZ);
            break;
        }
      }
    }
}

void InterfaceLoop(){
  if(progress == InstructionsCreated){
    if(isPosReached && !IsCurrentMotorMoving()){
    isPosReached = false;
    
      switch(currentInstruction->count){
        case 0:   
          if(currentInstruction->type == normalInst){
            
            savedInstruction = currentInstruction;
            currentInstruction = PickUpBrick(currentInstruction->brick);
            newInstruction = true;
            //Serial.println(111111);  
            break;
          }
          //Serial.println(111112);
          currentInstruction->count = 1;
          ChangeMotor(&myController, motorZ);
          
          break;
        case 1:
          if(currentInstruction->type == pickUp){
            GrabBrick(&myController);
            free(currentInstruction);
            currentInstruction = savedInstruction;
            currentInstruction->type = place;
            ChangeMotor(&myController, motorX);
            //Serial.println(222221);
            break;
          }
            digitalWrite(gearPin, HIGH);
          //Serial.println(222222);
          PlaceBrick(&myController);
          //ResetSystem();
          NextInstruction();
          
          break;
      }
      if(!queueIsEmpty){
        StartMotor();
      }
    }
  }
}



