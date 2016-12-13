/*Defined values. Remember they're used in an array, so -1 for actual values!*/ 
#define MaxX 15 
#define MaxY 3 
#define MaxZ 15 
#define OFFSETX 0
#define OFFSETZ 65
#define InstErr -1
#define ArrMin 0


#include "InputHandler.h"
enum brickState
{
	empty, notPlaced, placed
};

struct Queue queue;

/*Contains a single position in 3d space*/ 
typedef struct Position{ 
  int16_t x; 
  int16_t y; 
  int16_t z; 
}; 
 
Position CreatePosition(int16_t x, int16_t y, int16_t z){ 
  Position newPosition; 
  newPosition.x; 
  newPosition.y; 
  newPosition.z; 
 
  return newPosition; 
} 

enum InstructionType {normalInst, pickUp, place};

struct Instruction{
  InstructionType type;
  int positions[2];
  int level; //hvis denne er -1 s� er instruktionen fejlslagen
  short int count;
  BrickType brick;
};


int ConvertToGearDegrees(float BrickCord){
  return ((int)((45.0/0.32)*0.8*BrickCord));
}

Instruction CreateInstruction(float x, float z,int y, BrickType brick){
  Instruction newInstruction;
  newInstruction.positions[0] = ConvertToGearDegrees(x)+OFFSETX;
  newInstruction.positions[1] = ConvertToGearDegrees(z)+OFFSETZ;
  newInstruction.level = y;
  newInstruction.brick = brick;
  newInstruction.count = 0;
  newInstruction.type = normalInst;
  return newInstruction;
}
int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
 
/*Contains a three-dimensional array of chars and te position of the brick pickup site*/ 
typedef struct Blueprint{
  byte pos[MaxX][MaxY][MaxZ]; 
  Position pickup; 
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
  Serial.println(freeRam());
  /*initializing pickup site position to (0, 0, 0)*/
  temp->pickup.x = 0; 
  temp->pickup.y = 0; 
  temp->pickup.z = 0;

  //Serial.print("Coord(1,1,1): ");
  //Serial.println(temp->pos[1][1][1]);
  
  return temp; 
}

enum notAllowedEnum
{
	allowed, right, left, down, up
};

Instruction GetInstruction(Blueprint * bp, Position * bpProgress, bool * skipCaseChecker) {
	Instruction inst;
	inst.brick = none;
	inst.count = 0;
	inst.level = 0;
	inst.positions[0] = 0;
	inst.positions[1] = 0;
	inst.type = normalInst;

	//Serial.print("GetInstruction:");
	//Serial.println(freeRam());
	bool skipCase = false;
	for (int8_t yAxis = bpProgress->y; yAxis < MaxY; yAxis++) // lol det skal v�re en int_8t her, ellers g�r den amok
	{
		if (yAxis % 2 == 1 && !(*skipCaseChecker))
		{
			skipCase = true;
		}
		if (yAxis % 2 == 0)
		{
			*skipCaseChecker = false;
		}
		
		for (int16_t zAxis = bpProgress->z; zAxis < MaxZ; zAxis++)
		{
			for (int16_t xAxis = bpProgress->x; xAxis < MaxX; xAxis++)
			{
				bpProgress->x = xAxis;
				bpProgress->y = yAxis;
				bpProgress->z = zAxis;
				switch (bp->pos[xAxis][yAxis][zAxis])
				{
				case notPlaced:
					switch (bp->pos[xAxis + 2][yAxis][zAxis])
					{
					case notPlaced:
						if (xAxis >= 0 && xAxis + 3 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= 0 && zAxis + 1 < MaxZ && !skipCase)
						{
							bp->pos[xAxis][yAxis][zAxis] = placed;
							bp->pos[xAxis + 1][yAxis][zAxis] = placed;
							bp->pos[xAxis + 2][yAxis][zAxis] = placed;
							bp->pos[xAxis + 3][yAxis][zAxis] = placed;
							bp->pos[xAxis][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 1][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 2][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 3][yAxis][zAxis + 1] = placed;
							//Serial.println("LB90");
							return CreateInstruction(xAxis + 1, zAxis, yAxis, largeBrick90);       //enum BrickType {smallBrick, largeBrick0, largeBrick90, none};																								   
							//Place big brick
							break;
						}
						else if (xAxis >= 0 && xAxis + 3 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= 0 && zAxis + 1 < MaxZ)
						{
							//Serial.println("lb90 skipcase");
							skipCase = false;
							*skipCaseChecker = true;
						}
					case placed: case empty: default:
						switch (bp->pos[xAxis - 2][yAxis][zAxis])
						{
						case notPlaced:
							if (xAxis < MaxX && xAxis - 3 >= ArrMin && yAxis < MaxY && yAxis >= ArrMin && zAxis < MaxZ && zAxis - 1 >= ArrMin && !skipCase)
							{
								bp->pos[xAxis][yAxis][zAxis] = placed;
								bp->pos[xAxis - 1][yAxis][zAxis] = placed;
								bp->pos[xAxis - 2][yAxis][zAxis] = placed;
								bp->pos[xAxis - 3][yAxis][zAxis] = placed;
								bp->pos[xAxis][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 1][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 2][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 3][yAxis][zAxis + 1] = placed;
								//Serial.println("-LB90");
								return  CreateInstruction(xAxis - 1, zAxis, yAxis, largeBrick90);
								//Place big brick
								break;
							}
							else if (xAxis < MaxX && xAxis - 3 >= ArrMin && yAxis < MaxY && yAxis >= ArrMin && zAxis < MaxZ && zAxis - 1 >= ArrMin)
							{
								//Serial.println("-lb90 skipcase");
								skipCase = false;
								*skipCaseChecker = true;
							}
								
						case placed: case empty: default:
							switch (bp->pos[xAxis][yAxis][zAxis + 2])
							{
							case notPlaced:
								if (xAxis >= ArrMin && xAxis + 1 < MaxX && yAxis >= ArrMin && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ && !skipCase)
								{
									bp->pos[xAxis][yAxis][zAxis] = placed;
									bp->pos[xAxis][yAxis][zAxis + 1] = placed;
									bp->pos[xAxis][yAxis][zAxis + 2] = placed;
									bp->pos[xAxis][yAxis][zAxis + 3] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 1] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 2] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 3] = placed;
									//Serial.println("LB0");
									return  CreateInstruction(xAxis, zAxis + 1, yAxis, largeBrick0);
									//Place big brick
									break;
								}
								else if (xAxis >= ArrMin && xAxis + 1 < MaxX && yAxis >= ArrMin && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ)
								{
									//Serial.println("lb0 skipcase");
									skipCase = false;
									*skipCaseChecker = true;
								}
										
							case placed: case empty: default:
								switch (bp->pos[xAxis][yAxis][zAxis - 2])
								{
								case notPlaced:
									if (xAxis >= 0 && xAxis + 1 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ && !skipCase )
									{
										bp->pos[xAxis][yAxis][zAxis] = placed;
										bp->pos[xAxis][yAxis][zAxis - 1] = placed;
										bp->pos[xAxis][yAxis][zAxis - 2] = placed;
										bp->pos[xAxis][yAxis][zAxis - 3] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 1] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 2] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 3] = placed;
										//Serial.println("-LB0");
										return CreateInstruction(xAxis, zAxis - 1, yAxis, largeBrick0);
										//Place big brick
										break;
									}
									else if (xAxis >= 0 && xAxis + 1 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ)
									{
										//Serial.println("-lb0 skipcase");
										skipCase = false;
										*skipCaseChecker = true;
									}
												
								case placed: case empty: default:
									if (xAxis >= ArrMin && xAxis + 1 < MaxX && yAxis >= ArrMin && yAxis < MaxY && zAxis >= ArrMin && zAxis + 1 < MaxZ)
									{
										bp->pos[xAxis][yAxis][zAxis] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis] = placed;
										bp->pos[xAxis][yAxis][zAxis+1] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis+1] = placed;
										//Serial.println("SB90");
										return CreateInstruction(xAxis, zAxis, yAxis, smallBrick);
										//place small brick
									}								

								}
							}
						}
					}
				}
				bpProgress->x = xAxis;
			}
			bpProgress->z = zAxis;
			bpProgress->x = ArrMin;
		}
		bpProgress->y = yAxis;
		bpProgress->z = ArrMin;
	}
	Serial.println("No instruction created.");
	return CreateInstruction(inst.positions[0], inst.positions[1], inst.level, inst.brick);//this should never happen
}

//Insert values according to the Wall it has been given
void makeBlueprint(Blueprint* BP, Wall* W, int D){
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

void* GetNextInstruction(){
  return pop(&queue);
}
}

