//Control that defines motor and a controler that contains the motors
//Have function that contains the controller or motor in them
//Have struct for position/3DVector and a brickData struckt
//Margins
#define ERROR_MARGIN1 2
#define ERROR_MARGIN2 40
#define ERROR_MARGIN3 100

#define SMALL_BRICK_DEPO_X 0
#define SMALL_BRICK_DEPO_Z 0
#define LARGE_BRICK_0_DEPO_X 0
#define LARGE_BRICK_0_DEPO_Z 1
#define LARGE_BRICK_90_DEPO_X 0
#define LARGE_BRICK_90_DEPO_Z 1
//Chanel is the current motor input Interupts
enum Chanels {motorY, motorX, motorZ, motorRotation};
enum MotorStates {forward, backward, hold};
enum BrickType {smallBrick, largeBrick0, largeBrick90, none};
#include "Model.h"

//All data struct
typedef struct Motor{
    volatile int pos;
    int maxPos;
    int minPos;
    int pin1;
    int pin2;
    MotorStates state;
    volatile int sig1;
    volatile int sig2;
};



typedef struct Controller{
  int targetPos;
  Motor motorX;
  Motor motorY;
  Motor motorZ;
  Motor *runningMotor;
};




//Counstructers for all the struct
Controller CreateController(Motor motorX, Motor motorY, Motor motorZ){
  Controller newController;
  newController.targetPos;
  newController.motorX = motorX;
  newController.motorY = motorY;
  newController.motorZ = motorZ;
  newController.runningMotor = &motorX;
  return newController;
}



//Function that create the motor with the values that a dynamic at the start
Motor CreateMotor(int maxPos, int pin1, int pin2){
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
void ChangeMotorState(MotorStates state, Motor* motor){
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
     /* if(motor->state == forward)
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
bool MoveTo(int pos, Controller *control){
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
void ChangeMotor(Controller *myControl, Chanels newMotor){
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

void GrabBrick(Controller *myController){
  ChangeMotorState(forward, &myController->motorY);
  delay(1000);
  ChangeMotorState(backward, &myController->motorY);
  delay(1000);
  ChangeMotorState(hold, &myController->motorY);
}

