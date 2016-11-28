#include "queue.h";
#include "Model.h";
#include "Error.h";

int wallInt[5];
int wallCount = 0;

int progress = 0;

Queue wallQueue;

String tempNumber = "";

/*Reads an input stream from the USB port. Divides the input into wall structs, enqueues these structs and returns a pointer to the queue*/
void readInput(int wallInt[], Queue* Walls){
  /*loop runs while there is input waiting to be processed*/
  struct Wall wallTemp = createWall();
    for(int i = 0; i < 5; i++){
      wallTemp.value[i] = wallInt[i];
      //Serial.println(wallTemp.value[i]);
  }
  push(Walls, &wallTemp);
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
    if(W->value[0] < W->value[2]){
      startval = W->value[0];
      endval = W->value[2];
    }
    else{
      startval = W->value[2];
      endval = W->value[0];
    }
    int endH = W->value[4];
    int zval = W->value[1];
    for(startval; startval <= endval; startval++){
      for(int startH = 0; startH > endH; startH++)
      BP->pos[startval][startH][zval] = 1;
    }
  }
  
  else{
    if(W->value[1] < W->value[3]){
      startval = W->value[1];
      endval = W->value[3];
    }
    else{
      startval = W->value[3];
      endval = W->value[1];
    }
    int endH = W->value[4];
    int xval = W->value[0];
    for(startval; startval <= endval; startval++){
      for(int startH = 0; startH > endH; startH++)
      BP->pos[xval][startH][startval] = 1;
    }
  }
}

/*Takes a pointer to queue containing Wall structs, converts these into a Blueprint struct and returns a pointer to said Blueprint struct*/
Blueprint* convertToBlueprint(Queue* WallQueuePointer){
  Wall* tempWall;
  
    Serial.println("Hej");
  struct Blueprint* tempBlueprint = createBlueprint();
  while(WallQueuePointer->size != 0){
    tempWall = pop(WallQueuePointer);
    if(tempWall->value[4] > 0 && tempWall->value[4] < MaxY //Checks wall height for 0 < h <= max height
      && tempWall->value[0] >= 0 && tempWall->value[0] > MaxX // Check if value is within model bounds
      && tempWall->value[1] >= 0 && tempWall->value[1] > MaxZ // Same
      && tempWall->value[2] >= 0 && tempWall->value[2] > MaxX //You get the idea
      && tempWall->value[3] >= 0 && tempWall->value[3] > MaxZ){ //not even gonna try
      if(tempWall->value[0] == tempWall->value[2]   // Checks if start X == end X (prevents diagonal walls)
      && tempWall->value[1] != tempWall->value[3]){ // Checks if wall is not 0 long
        makeBlueprint(tempBlueprint, tempWall, 0);
      }
      if(tempWall->value[1] == tempWall->value[3]   // Checks if start y == end y (prevents diagonal walls)
      && tempWall->value[0] != tempWall->value[2]){ // Checks if wall is not 0 long
        makeBlueprint(tempBlueprint, tempWall, 1);
      }
      else
        ErrorCode(ERR_WALL);
    }
    else
      ErrorCode(ERR_WALL);
  }

  Serial.println("Coord: " + tempBlueprint->pos[1][0][1]);
  return tempBlueprint;
}

