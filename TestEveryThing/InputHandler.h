#include "Error.h"
#include "queue.h"

int wallInt[5];
int wallCount = 0;

int progress = 0;

Queue wallQueue;

String tempNumber = "";

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
    progress = 1;
  }
}

//Insert values according to the Wall it has been given
makeBlueprint(Blueprint* BP, Wall* W, int D){
  int startval, endval;
  if(D){
    if(W->value[StartX] < W->value[EndX]){
      startval = W->value[StartX];
      endval = W->value[EndX];
    }
    else{
      startval = W->value[EndX];
      endval = W->value[StartX];
    }
    int endH = W->value[Height];
    int zval = W->value[StartZ];
    for(startval; startval <= endval; startval++){
      for(int startH = 0; startH < endH; startH++)
        BP->pos[startval][startH][zval] = 1;
    }
  }
  
  else{
    if(W->value[StartZ] < W->value[EndZ]){
      startval = W->value[StartZ];
      endval = W->value[EndZ];
    }
    else{
      startval = W->value[EndZ];
      endval = W->value[StartZ];
    }
    int endH = W->value[Height];
    int xval = W->value[StartX];
    for(startval; startval <= endval; startval++){
      for(int startH = 0; startH < endH; startH++)
      BP->pos[xval][startH][startval] = 1;
    }
  }
}

/*Takes a pointer to queue containing Wall structs, converts these into a Blueprint struct and returns a pointer to said Blueprint struct*/
Blueprint* convertToBlueprint(Queue* WallQueuePointer){
	Wall* tempWall;

  struct Blueprint* tempBlueprint = createBlueprint();

  while(WallQueuePointer->size != 0){

    tempWall = (Wall*)pop(WallQueuePointer);
    if(tempWall->value[Height] > 0 && tempWall->value[Height] <= MaxY //Checks wall height for 0 < h <= max height
      && tempWall->value[StartX] >= 0 && tempWall->value[StartX] < MaxX // Check if value is within model bounds
      && tempWall->value[StartZ] >= 0 && tempWall->value[StartZ] < MaxZ // Same
      && tempWall->value[EndX] >= 0 && tempWall->value[EndX] < MaxX //You get the idea
      && tempWall->value[EndZ] >= 0 && tempWall->value[EndZ] < MaxZ){ //not even gonna try
      if(tempWall->value[StartX] == tempWall->value[EndX]   // Checks if start X == end X (prevents diagonal walls)
      && tempWall->value[StartZ] != tempWall->value[EndZ]){ // Checks if wall is not 0 long
        makeBlueprint(tempBlueprint, tempWall, 0);
      }
      else if(tempWall->value[StartZ] == tempWall->value[EndZ]   // Checks if start Z == end Z (prevents diagonal walls)
      && tempWall->value[StartX] != tempWall->value[EndX]){ // Checks if wall is not 0 long
        makeBlueprint(tempBlueprint, tempWall, 1);
      }
      else{
        ErrorCode(ERR_WALL);
      }
    }
    else{
      ErrorCode(ERR_WALL);
    }
  }
  return tempBlueprint;
}

