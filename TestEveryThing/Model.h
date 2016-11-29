/*Defined values. Remember they're used in an array, so -1 for actual values!*/ 
#define MaxX 26 
#define MaxY 3 
#define MaxZ 15 

enum wallSpecs {
  StartX,
  StartZ,
  EndX,
  EndZ,
  Height
} wallSpecs;

/*Contains a single position in 3d space*/ 
typedef struct position{ 
  byte x; 
  byte y; 
  byte z; 
}; 
 
position CreatePosition(byte x, byte y, byte z){ 
  position newPosition; 
  newPosition.x; 
  newPosition.y; 
  newPosition.z; 
 
  return newPosition; 
} 

enum InstructionType {normal, pickUp, place};

struct Instruction{
  InstructionType type;
  int positions[2];
  short int count;
  BrickType brick;
};


int ConvertToGearDegrees(int BrickCord){
  return (int)((15.0/0.32)*0.8*(float)BrickCord);
}

Instruction CreateInstruction(int x, int z,int y, BrickType brick){
  Instruction newInstruction;
  newInstruction.positions[0] = ConvertToGearDegrees(x);
  newInstruction.positions[1] = ConvertToGearDegrees(z);
  newInstruction.brick = brick;
  newInstruction.count = 0;
  newInstruction.type = normal;
  return newInstruction;
}

 
/*Contains a three-dimensional array of chars and te position of the brick pickup site*/ 
typedef struct Blueprint{
  byte pos[MaxX][MaxY][MaxZ]; 
  position pickup; 
}; 
 
Blueprint* createBlueprint(){
  struct Blueprint* temp = malloc(sizeof(Blueprint));
  /*initializing all values in the array to 0*/ 
  for(int a = 0; a < MaxX; a++){ 
    for(int b = 0; b < MaxY; b++){ 
      for(int c = 0; c < MaxZ; c++){ 
        temp->pos[a][b][c] = 0;
      } 
    }
  }
  /*initializing pickup site position to (0, 0, 0)*/
  temp->pickup.x = 0; 
  temp->pickup.y = 0; 
  temp->pickup.z = 0;

  Serial.print("Coord(1,1,1): ");
  Serial.println(temp->pos[1][1][1]);
  
  return temp; 
}

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


