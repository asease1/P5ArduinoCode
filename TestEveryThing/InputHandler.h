#include "queue.h";
#include "Model.h";

struct Wall{
  char value[5];
};

/*creates a new instance of the wall struct*/
struct Wall createWall(){
  struct Wall tempWall;
  tempWall.value[0] = '0';
  tempWall.value[1] = '0';
  tempWall.value[2] = '0';
  tempWall.value[3] = '0';
  tempWall.value[4] = '0';
  return tempWall;
}

/*Reads an input stream from the USB port. Divides the input into wall structs, enqueues these structs and returns a pointer to the queue*/
int readInput(){
  Queue Walls = createQueue();
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

/*Takes a pointer to queue containing Wall structs, converts these into a Blueprint struct and returns a pointer to said Blueprint struct*/

