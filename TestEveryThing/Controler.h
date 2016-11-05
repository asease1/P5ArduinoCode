//Control that defines motor and a controler that contains the motors
//Have function that contains the controller or motor in them
//Have struct for position/3DVector and a brickData struckt

//Chanel is the current motor input Interupts
enum Chanels {motorY, motorX, motorZ, motorRotation};
enum Motorstates {forward, backward, hold};
enum BrickType {smallBrick, largeBrick0, largeBrick90};

//All data struct
struct Motor{
    volatile int pos;
    int maxPos;
    int minPos;
    int pin1;
    int pin2;
    Motorstates state;
    volatile int sig1;
    volatile int sig2;
};

struct Controller{
  int targetPos;
  Motor motorX;
  Motor motorY;
  Motor motorZ;
  Motor *runningMotor;
  //if null we dont have any brick picked
  Brick currentBrick;
};

struct Position{
  int x;
  int y;
  int z;
};

struct Brick{
  Position myPos;
  BrickType myType;
};

//Counstructers for all the struct
Controller CreateController(Motor motorX, Motor motorY, Motor motorZ){
  Controller newController;
  newController.targetPos
  newController.motorX = motorX;
  newController.motorY = motorY;
  newController.motorZ = motorZ;
  newController.runningMotor = &motorX;
  newController.currentBrick = null;

  return newController;
}


Position CreatePosition(int x, int y, int z){
  Position newPosition;
  newPosition.x;
  newPosition.y;
  newPosition.z;

  return newPosition;
}

Brick CreateBrick(Position pos, BrickType brickType){
  Brick newBrick;
  newBrick.myPos = pos;
  newBrick.myType = brickType;

  return newBrick;
}


//Function that create the motor with the values that a dynamic at the start
Motor CreateMotor(int maxPos, int pin1, int pin2){
  Motor newMotor;
  newMotor.pos = 0;
  newMotor.maxPos = maxPos;
  newMotor.minPos = minPos;
  newMotor.pin1 = pin1;
  newMotor.pin2 = pin2;
  newMotor.state = hold;
  //Can check the sig pins in here if you chance chanel to get a more precise position.
  newMotor.sig1 = 0;
  newMotor.sig2 = 0;

  return newMotor;
}

//Update the controller targetpos to the new position and start the runningMotor in the dircation of target
void MoveTo(int pos, Controller control){
  if(control.runningMotor->pos > (pos + ERROR_MARGIN)){
      ChangeMotorState(backward, control.runningMotor);
  }
  else if(control.runningMotor->pos < (pos - ERROR_MARGIN)){
    ChangeMotorState(forward, control.runningMotor);
  }

  control.targetPos = pos;
}

//Take a motor and set the pins to the diraction you want it to move and chance the state
//Have to watch out for that you can run a motor that the channel is not on if this is don't you cannot update its position.
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
      //we should find a other way, maybe use PVM and gears so we slow down.
      delay(Hold_Delay);
    
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, LOW);
      motor->state = hold;

      //Debug
      //MoveTo(motor->pos * -1, runningMotor);
      break;
  }
}

//Switch the runningMotor on the Controller to a new Motor
void ChangeMotor(Controller myControl, Chanels newMotor){
  switch(newMotor){
    case motorX:
      digitalWrite(chanelPin1, HIGH);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      myControl.runningMotor = &myControl.motorX;
      break;
    case motorY:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, LOW);
      digitalWrite(chanelPin3, LOW);
      myControl.runningMotor = &myControl.motorY;
      break;
    case motorZ:
      digitalWrite(chanelPin1, LOW);
      digitalWrite(chanelPin2, HIGH);
      digitalWrite(chanelPin3, LOW);
      myControl.runningMotor = &myControl.motorZ;
      break;
    case motorRotation:
      //place code for rotationMotor.
      break;
  }
}

