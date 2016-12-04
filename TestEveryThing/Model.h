/*Defined values. Remember they're used in an array, so -1 for actual values!*/ 
#define MaxX 15 
#define MaxY 3 
#define MaxZ 15
#define ArrMin 0
#include <HardwareSerial.h>

enum wallSpecs {
  StartX,
  StartZ,
  EndX,
  EndZ,
  Height
} wallSpecs;

enum brickState
{
	empty, notPlaced, placed
};

/*Contains a single position in 3d space*/ 
typedef struct Position{ 
  byte x; 
  byte y; 
  byte z; 
}; 
 
Position CreatePosition(byte x, byte y, byte z){ 
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
  int level;
  short int count;
  BrickType brick;
};


int ConvertToGearDegrees(int BrickCord){
  return (int)((15.0/0.32)*0.8*(float)BrickCord);
}

Instruction CreateInstruction(int x, int z,int y, BrickType brick){
  Instruction newInstruction;
  Serial.print(x);
  Serial.print(", ");
  Serial.print(z);
  Serial.println(" BCOR");
  newInstruction.positions[0] = ConvertToGearDegrees(x);
  newInstruction.positions[1] = ConvertToGearDegrees(z);
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

  Serial.print("Coord(1,1,1): ");
  Serial.println(temp->pos[1][1][1]);
  
  return temp; 
}

struct Wall{
  byte value[5];
};

Instruction GetInstruction(Blueprint * bp, Position * bpProgress) {
	Instruction inst;
	Serial.println("GetInstruction");
	for (int yAxis = bpProgress->y; yAxis < MaxY; yAxis++)
	{
		for (int zAxis = bpProgress->z; zAxis < MaxZ; zAxis++)
		{
			for (int xAxis = bpProgress->x; xAxis < MaxX; xAxis++)
			{
				switch (bp->pos[xAxis][yAxis][zAxis])
				{
				case notPlaced:
					switch (bp->pos[xAxis + 2][yAxis][zAxis])
					{
					case notPlaced:
						if (xAxis >= 0 && xAxis + 3 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= 0 && zAxis + 1 < MaxZ)
						{
							bp->pos[xAxis][yAxis][zAxis] = placed;
							bp->pos[xAxis + 1][yAxis][zAxis] = placed;
							bp->pos[xAxis + 2][yAxis][zAxis] = placed;
							bp->pos[xAxis + 3][yAxis][zAxis] = placed;
							bp->pos[xAxis][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 1][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 2][yAxis][zAxis + 1] = placed;
							bp->pos[xAxis + 3][yAxis][zAxis + 1] = placed;
							bpProgress->x = xAxis;
							bpProgress->y = yAxis;
							bpProgress->z = zAxis;
							Serial.println("LB90");
							return CreateInstruction(xAxis + 1, zAxis, yAxis, largeBrick90);       //enum BrickType {smallBrick, largeBrick0, largeBrick90, none};																								   
							//Place big brick
							break;
						}

					case placed: case empty:
						switch (bp->pos[xAxis - 2][yAxis][zAxis])
						{
						case notPlaced:
							if (xAxis < MaxX && xAxis - 3 >= ArrMin && yAxis < MaxY && yAxis >= ArrMin && zAxis < MaxZ && zAxis - 1 >= ArrMin)
							{
								bp->pos[xAxis][yAxis][zAxis] = placed;
								bp->pos[xAxis - 1][yAxis][zAxis] = placed;
								bp->pos[xAxis - 2][yAxis][zAxis] = placed;
								bp->pos[xAxis - 3][yAxis][zAxis] = placed;
								bp->pos[xAxis][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 1][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 2][yAxis][zAxis + 1] = placed;
								bp->pos[xAxis - 3][yAxis][zAxis + 1] = placed;
								bpProgress->x = xAxis;
								bpProgress->y = yAxis;
								bpProgress->z = zAxis;
								Serial.println("LB90");
								return  CreateInstruction(xAxis - 1, zAxis, yAxis, largeBrick90);
								//Place big brick
								break;
							}
								
						case placed: case empty:
							switch (bp->pos[xAxis][yAxis][zAxis + 2])
							{
							case notPlaced:
								if (xAxis >= ArrMin && xAxis + 1 < MaxX && yAxis >= ArrMin && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ)
								{
									bp->pos[xAxis][yAxis][zAxis] = placed;
									bp->pos[xAxis][yAxis][zAxis + 1] = placed;
									bp->pos[xAxis][yAxis][zAxis + 2] = placed;
									bp->pos[xAxis][yAxis][zAxis + 3] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 1] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 2] = placed;
									bp->pos[xAxis + 1][yAxis][zAxis + 3] = placed;
									bpProgress->x = xAxis;
									bpProgress->y = yAxis;
									bpProgress->z = zAxis;
									Serial.println("LB0");
									return  CreateInstruction(xAxis, zAxis + 1, yAxis, largeBrick0);
									//Place big brick
									break;
								}
										
							case placed: case empty:
								switch (bp->pos[xAxis][yAxis][zAxis - 2])
								{
								case notPlaced:
									if (xAxis >= 0 && xAxis + 1 < MaxX && yAxis >= 0 && yAxis < MaxY && zAxis >= ArrMin && zAxis + 3 < MaxZ)
									{
										bp->pos[xAxis][yAxis][zAxis] = placed;
										bp->pos[xAxis][yAxis][zAxis - 1] = placed;
										bp->pos[xAxis][yAxis][zAxis - 2] = placed;
										bp->pos[xAxis][yAxis][zAxis - 3] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 1] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 2] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis - 3] = placed;
										bpProgress->x = xAxis;
										bpProgress->y = yAxis;
										bpProgress->z = zAxis;
										Serial.println("LB0");
										return CreateInstruction(xAxis, zAxis - 1, yAxis, largeBrick0);
										//Place big brick
										break;
									}
												
								case placed: case empty:
									if (xAxis >= ArrMin && xAxis + 1 < MaxX && yAxis >= ArrMin && yAxis < MaxY && zAxis >= ArrMin && zAxis + 1 < MaxZ)
									{
										bp->pos[xAxis][yAxis][zAxis] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis] = placed;
										bp->pos[xAxis][yAxis][zAxis + 1] = placed;
										bp->pos[xAxis + 1][yAxis][zAxis + 1] = placed;
										bpProgress->x = xAxis;
										bpProgress->y = yAxis;
										bpProgress->z = zAxis;
										Serial.println("SB90");
										return CreateInstruction(xAxis, zAxis, yAxis, smallBrick);
										//place small brick
									}
									else
									{
										//No brick can be placed
									}
												
								default:
									break;
								}
										
							default:
								break;
							}
								
						default:
							break;
						}
					default:
						break;
					}
				default:
					break;
				}
			}
		}
	}

	return inst;//this should never happen
}
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
