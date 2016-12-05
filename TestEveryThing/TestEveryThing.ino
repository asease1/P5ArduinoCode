//Interrupts pin 2-3
#define interupt1 2
#define interupt2 3
//Dircation pins x axis
#define xPin1 4
#define xPin2 5
//Dircation pins y axis
#define yPin1 6
#define yPin2 7
//Diraction pins z axis
#define zPin1 8
#define zPin2 13
//Chanel Pins
#define chanelPin3 10
#define chanelPin2 11
#define chanelPin1 12


//gear are the amount of speed the motor should move at from 0-255
#define gear1 255
#define gear2 110
#define gear3 140

#define Hold_Delay 500
#define DELAY_FOR_MOTOR_MOVEMENT 200

#define MAX_QUEUE_SIZE 1

#include "Wire.h"
#include "Controller.h"
#include "InputHandler.h"

struct Instruction* currentInstruction;
struct Instruction* savedInstruction;
struct Queue queue;
Blueprint* bp;
Position BPProgress;
struct Controller myController;

bool isResat = false;
bool queueIsEmpty = true;
bool isPosReached = false;
bool newInstruction = false;

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
  
  myController = CreateController(CreateMotor(1050, xPin1, xPin2),CreateMotor(1050, yPin1, yPin2),CreateMotor(1050, zPin1, zPin2));

  
  //push(&queue, &CreateInstruction(5,5,0, smallBrick));
  //push(&queue, &CreateInstruction(12,12,0, smallBrick));
  NextInstruction();
  
  
  ResetSystem();
  //analogWrite(gearPin, 120);
  //StartMotor();
  isPosReached = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(progress == 0){ // run input handler
    beginInputHandler();
  }
  if(progress == 1){
    bp = convertToBlueprint(&wallQueue);
    Serial.print("Coord(1,1,1): ");
    Serial.println(bp->pos[1][1][1]);
    progress = 2;
  }
  if (progress == 2)
  {
	  if (queue.size < MAX_QUEUE_SIZE) {
		  for (int i = 0; i < MaxY; i++)
		  {
			  Serial.println("");
			  for (int j = 0; j < MaxZ; j++)
			  {
				  Serial.println("");
				  for (int k = 0; k < MaxX; k++)
				  {
					  Serial.print(bp->pos[k][i][j]);
				  }
			  }
		  }
		  Serial.println("");
		  //Serial.print(queue.size);
		  //Serial.println(" queue size");
		  Serial.println("Before GetInstruction");
		  Instruction* inst = &GetInstruction(bp, &BPProgress);
		  push(&queue, inst);
		  Serial.print(BPProgress.x);
		  Serial.print(", ");
		  Serial.print(BPProgress.z);
		  Serial.print(", ");
		  Serial.print(BPProgress.y);
		  Serial.println(" progresspoint");
		  Serial.print(inst->brick);
		  Serial.println(" brick");
		  Serial.print(inst->positions[0]);
		  Serial.println(" x");
		  Serial.print(inst->positions[1]);
		  Serial.println(" z");
		  Serial.print(inst->level);
		  Serial.println(" y");

		  Serial.println("");
	  }
  }
  //if(queue.size < MAX_QUEUE_SIZE)
    //push(&queue, &GetInstrction());
  //put your main code here, to run repeatedly:
  //Serial.println(queue.size);
  //Serial.println(isPosReached);
  
  //if(isPosReached && !IsCurrentMotorMoving()){
  //  isPosReached = false;
  //  //Serial.println(CheckPositionMargen()); 
  //  //Serial.println(myController.runningMotor->pos);
  //  //Serial.println(currentInstruction->positions[currentInstruction->count]); 
  // /*if(!CheckPositionMargen()){
  //    Serial.println("test");
  //    //Start motor to get closer to target if we have moved to far;
  //    MoveTo(currentInstruction->positions[currentInstruction->count], &myController);
  //  }
  //  else{
  //   */    
  //    //Serial.println(currentInstruction->count);
  //    //Serial.println(currentInstruction->type);
  //    switch(currentInstruction->count){
  //      case 0:   
  //        if(currentInstruction->type == normalInst){
  //          savedInstruction = currentInstruction;
  //          currentInstruction = PickUpBrick(smallBrick);
  //          newInstruction = true;
  //          //Serial.println(111111);  
  //          break;
  //        }
  //        //Serial.println(111112);
  //        currentInstruction->count = 1;
  //        ChangeMotor(&myController, motorZ);
  //        break;
  //      case 1:
  //        if(currentInstruction->type == pickUp){
  //          GrabBrick(&myController);
  //          free(currentInstruction);
  //          currentInstruction = savedInstruction;
  //          currentInstruction->type = place;
  //          ChangeMotor(&myController, motorX);
  //          //Serial.println(222221);
  //          break;
  //        }
  //          digitalWrite(gearPin, HIGH);
  //        //Serial.println(222222);
  //        PlaceBrick(&myController);
  //        NextInstruction();
  //        
  //        break;
  //    }
  //    if(!queueIsEmpty){
  //      StartMotor();
  //    }
  //}
    
 //Serial.println(myController.runningMotor->pos);
 //delay(10);
 //Serial.println(freeRam());
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
            analogWrite(gearPin, gear2+10);
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
            analogWrite(gearPin, gear2+10);
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

void NextInstruction(){
  ChangeMotor(&myController, motorX);
  free(currentInstruction);

  if(queue.size == 0)
    queueIsEmpty = true;
  else
    queueIsEmpty = false;
  currentInstruction = (Instruction*)pop(&queue);
}

void ResetSystem(){
  isResat = false;
  analogWrite(gearPin, 180);
  ResetMotor(motorY);
  analogWrite(gearPin, 180);
  ResetMotor(motorZ);
  analogWrite(gearPin, 180);
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
  ChangeMotor(&myController, motor);
  ChangeMotorState(backward, myController.runningMotor);
  TimeSinceLastInterrupt = millis();
  while(IsCurrentMotorMoving()){
    //Serial.println(myController.runningMotor->pos);
  }
  ChangeMotorState(hold, myController.runningMotor);
  myController.runningMotor->pos = 0;
}





