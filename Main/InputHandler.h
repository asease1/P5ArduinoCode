#include "Error.h"
#include "queue.h"

enum LoopState{ //Used to keep track of what state the program is in
	Inital, 
	InputRecieved, 
	BlueprintCreated, 
	InstructionsCreated, 
	ReadyToRun
};
enum wallSpecs { //Used in the generation of walls
  StartX,
  StartZ,
  EndX,
  EndZ,
  Height
};

int wallInt[5];
int wallCount = 0;

LoopState progress = Inital;

Queue wallQueue; //As the walls are generated, they are kept in this queue

String tempNumber = "";




struct Wall{ 
  byte value[5];
};

/*Creates a new instance of the wall struct*/
Wall* createWall(){
  struct Wall* tempWall = malloc(sizeof(Wall));
  tempWall->value[StartX] = 0; // Wall starting x
  tempWall->value[StartZ] = 0; // Wall starting z
  tempWall->value[EndX] = 0; // Wall stopping x
  tempWall->value[EndZ] = 0; // Wall stopping z
  tempWall->value[Height] = 0; // Wall height
  return tempWall;
}
/*Reads an input stream from the serial port, usually USB. Divides the input into wall structs, enqueues these structs and returns a pointer to the queue*/
void readInput(int wallInt[], Queue* Walls){
  Wall* wallTemp = createWall();
  wallTemp->value[StartX] = wallInt[StartX];
  wallTemp->value[StartZ] = wallInt[StartZ];
  wallTemp->value[EndX] = wallInt[EndX];
  wallTemp->value[EndZ] = wallInt[EndZ];
  wallTemp->value[Height] = wallInt[Height];
  push(Walls, wallTemp);
}


DecodeInput(){ //A kind of parser for the input
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
  if(progress == 0){ // Run input handler until we have recieved some meaningfull input
    DecodeInput();
  }
}
