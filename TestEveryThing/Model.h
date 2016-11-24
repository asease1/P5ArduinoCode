/*Defined values. Remember they're used in an array, so -1 for actual values!*/ 
#define MaxX 26 
#define MaxY 3 
#define MaxZ 15 
 
/*Contains a single position in 3d space*/ 
struct Position{ 
  char x; 
  char y; 
  char z; 
}; 
 
Position CreatePosition(char x, char y, char z){ 
  Position newPosition; 
  newPosition.x; 
  newPosition.y; 
  newPosition.z; 
 
  return newPosition; 
} 

enum InstructionType {normal, pickUp, place};

struct Instruction{
  InstructionType type;
  int positions[2];
  int level;
  short int count;
  BrickType brick;
};


int ConvertToGearDegrees(int BrickCord){
  //return 100;
  Serial.println(((15.0/0.32)*0.8*(float)BrickCord));
  return (int)((15.0/0.32)*0.8*(float)BrickCord);
}

Instruction CreateInstruction(int x, int z, int level, BrickType brick){
  Instruction newInstruction;
  newInstruction.positions[0] = ConvertToGearDegrees(x);
  newInstruction.positions[1] = ConvertToGearDegrees(z);
  newInstruction.level = level;
  newInstruction.brick = brick;
  newInstruction.count = 0;
  newInstruction.type = normal;
  return newInstruction;
}

 
/*Contains a three-dimensional array of chars and te position of the brick pickup site*/ 
typedef struct Blueprint{ 
  char pos[MaxX][MaxY][MaxZ]; 
  Position pickup; 
}; 
 
Blueprint* createBlueprint(){ 
  struct Blueprint* temp = malloc(sizeof(Blueprint)); 
  struct Position ps; 
  char a, b, c; 
  /*initializing all values in the array to 0*/ 
  for(a = 0; a < MaxX; a++){ 
    for(b = 0; b < MaxY; b++){ 
      for(c = 0; c < MaxZ; c++){ 
        temp->pos[a][b][c] = 0; 
        } 
      } 
    } 
  /*initializing pickup site position to (0, 0, 0)*/ 
  temp->pickup.x = 0; 
  temp->pickup.y = 0; 
  temp->pickup.z = 0; 
  return temp; 
}



enum brickState
{
	empty, notPlaced, placed
};

Instruction GetInstruction(Blueprint bp, Position * bpProgress) {
	Instruction inst;

	for (int yAxis = bpProgress->y; yAxis < MaxY; yAxis++)
	{
		for (int zAxis = bpProgress->z; zAxis < MaxZ; zAxis += 2)
		{
			for (int xAxis = bpProgress->x; xAxis < MaxX; xAxis += 2)
			{
				switch (bp.pos[xAxis][yAxis][zAxis])
				{
				case notPlaced:
					switch (bp.pos[xAxis + 2][yAxis][zAxis])
					{
					case notPlaced:
						bp.pos[xAxis][yAxis][zAxis] = placed;
						bp.pos[xAxis + 2][yAxis][zAxis] = placed;
						bpProgress->x = xAxis;
						bpProgress->y = yAxis;
						bpProgress->z = zAxis;
						return CreateInstruction(xAxis+1, zAxis, yAxis, largeBrick90);       //enum BrickType {smallBrick, largeBrick0, largeBrick90, none};
						//Place big brick
						break;
					case placed: case empty:
						switch (bp.pos[xAxis - 2][yAxis][zAxis])
						{
						case notPlaced:
							bp.pos[xAxis][yAxis][zAxis] = placed;
							bp.pos[xAxis - 2][yAxis][zAxis] = placed;
							bpProgress->x = xAxis;
							bpProgress->y = yAxis;
							bpProgress->z = zAxis;
							return  CreateInstruction(xAxis-1, zAxis, yAxis, largeBrick90);
							//Place big brick
							break;
						case placed: case empty:
							switch (bp.pos[xAxis][yAxis + 2][zAxis])
							{
							case notPlaced:
								bp.pos[xAxis][yAxis][zAxis] = placed;
								bp.pos[xAxis][yAxis][zAxis + 2] = placed;
								bpProgress->x = xAxis;
								bpProgress->y = yAxis;
								bpProgress->z = zAxis;
								return  CreateInstruction(xAxis, zAxis+1, yAxis, largeBrick0);
								//Place big brick
								break;
							case placed: case empty:
								switch (bp.pos[xAxis][yAxis - 2][zAxis])
								{
								case notPlaced:
									bp.pos[xAxis][yAxis][zAxis] = placed;
									bp.pos[xAxis][yAxis - 2][zAxis] = placed;
									bpProgress->x = xAxis;
									bpProgress->y = yAxis;
									bpProgress->z = zAxis;
									CreateInstruction(xAxis, zAxis-1, yAxis, largeBrick0);
									//Place big brick
									break;
								case placed: case empty:
									bpProgress->x = xAxis;
									bpProgress->y = yAxis;
									bpProgress->z = zAxis;
									CreateInstruction(xAxis, zAxis, yAxis, smallBrick);
									//place small brick
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


