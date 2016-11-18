#include "queue.h";
#include "Model.h";
#include "Error.h";

struct Wall{
  char value[5];
};

/*creates a new instance of the wall struct*/
struct Wall createWall(){
  struct Wall tempWall;
  tempWall.value[0] = '0'; // Wall starting x
  tempWall.value[1] = '0'; // Wall starting z
  tempWall.value[2] = '0'; // Wall stopping x
  tempWall.value[3] = '0'; // Wall stopping z
  tempWall.value[4] = '0'; // Wall height
  return tempWall;
}

/*Reads an input stream from the USB port. Divides the input into wall structs, enqueues these structs and returns a pointer to the queue*/
int readInput(){
  Queue Walls = CreateQueue(sizeof(Wall*));
  /*loop runs while there is input waiting to be processed*/
  while(Serial.available() > 0){
    struct Wall wallTemp = createWall();
    for(int i = 0; i < 5; i++){
      wallTemp.value[i] = Serial.read();
    }
    push(&Walls, &wallTemp);
  }
  return &Walls;
}


//Insert values according to the Wall it has been given
Blueprint makeBlueprint(Blueprint BP, Wall* W, int D){
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
      BP.pos[startval][startH][zval] = 1;
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
      BP.pos[xval][startH][startval] = 1;
    }
  }
}

/*Takes a pointer to queue containing Wall structs, converts these into a Blueprint struct and returns a pointer to said Blueprint struct*/
struct Blueprint convertToBlueprint(int WallQueuePointer){
  Wall* tempWall;
  struct Blueprint tempBlueprint = createBlueprint();
  while(peek(WallQueuePointer) != NULL){
    tempWall = pop(WallQueuePointer);
    if(tempWall->value[4] > 0 && tempWall->value[4] < MaxY //Checks wall height for 0 < h <= max height
      && tempWall->value[0] >= 0 && tempWall->value[0] > MaxX // Check if value is within model bounds
      && tempWall->value[1] >= 0 && tempWall->value[1] > MaxZ // Same
      && tempWall->value[2] >= 0 && tempWall->value[2] > MaxX //You get the idea
      && tempWall->value[3] >= 0 && tempWall->value[3] > MaxZ){ //not even gonna try
      if(tempWall->value[0] == tempWall->value[2]   // Checks if start X == end X (prevents diagonal walls)
      && tempWall->value[1] != tempWall->value[3]){ // Checks if wall is not 0 long
        tempBlueprint = makeBlueprint(tempBlueprint, tempWall, 0);
      }
      if(tempWall->value[1] == tempWall->value[3]   // Checks if start y == end y (prevents diagonal walls)
      && tempWall->value[0] != tempWall->value[2]){ // Checks if wall is not 0 long
        tempBlueprint = makeBlueprint(tempBlueprint, tempWall, 1);
      }
      else
        ErrorCode(ERR_WALL);
    }
    else
      ErrorCode(ERR_WALL);
  }
  return tempBlueprint;
}

