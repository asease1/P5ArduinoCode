#include "Error.h"
#include "queue.h"

enum LoopState{Inital, InputRecieved, BlueprintCreated, InstructionsCreated, ReadyToRun};
enum wallSpecs {
  StartX,
  StartZ,
  EndX,
  EndZ,
  Height
};

int wallInt[5];
int wallCount = 0;

LoopState progress = Inital;

Queue wallQueue;

String tempNumber = "";




struct Wall{
  byte value[5];
};

/*creates a new instance of the wall struct*/
Wall* createWall(){
  struct Wall* tempWall = malloc(sizeof(Wall));
  tempWall->value[StartX] = 0; // Wall starting x
  tempWall->value[StartZ] = 0; // Wall starting z
  tempWall->value[EndX] = 0; // Wall stopping x
  tempWall->value[EndZ] = 0; // Wall stopping z
  tempWall->value[Height] = 0; // Wall height
  return tempWall;
}
/*Reads an input stream from the USB port. Divides the input into wall structs, enqueues these structs and returns a pointer to the queue*/
void readInput(int wallInt[], Queue* Walls){
  /*loop runs while there is input waiting to be processed*/
  Wall* wallTemp = createWall();
  wallTemp->value[StartX] = wallInt[StartX];
  wallTemp->value[StartZ] = wallInt[StartZ];
  wallTemp->value[EndX] = wallInt[EndX];
  wallTemp->value[EndZ] = wallInt[EndZ];
  wallTemp->value[Height] = wallInt[Height];
  //Serial.println(wallTemp->value[??]);
  push(Walls, wallTemp);
}

beginInputHandler(){
  if(Serial.available()){
    if((char)Serial.peek() != 'k'){
      tempNumber = tempNumber + (char)Serial.read();
    }
    else{
      Serial.read();
      wallInt[(wallCount++%5)] = (byte)tempNumber.toInt();
      tempNumber = "";
    }
  }
  if(wallCount%5 == 0 && wallCount != 0){
    readInput(wallInt, &wallQueue);
    wallCount = 0;
  }
  if((char)Serial.peek() == 'e'){
    Serial.read();
    progress = InputRecieved;
  }
}


void InputHandlerLoop(){
  if(progress == 0){ // run input handler
    beginInputHandler();
  }
}
