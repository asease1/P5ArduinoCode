/*Defined values. Remember they're used in an array, so -1 for actual values!*/ 
#define MaxX 26 
#define MaxY 3 
#define MaxZ 15 
 


/*Contains a single position in 3d space*/ 
typedef struct position{ 
  char x; 
  char y; 
  char z; 
}; 
 
position CreatePosition(char x, char y, char z){ 
  position newPosition; 
  newPosition.x; 
  newPosition.y; 
  newPosition.z; 
 
  return newPosition; 
} 
 
/*Contains a three-dimensional array of chars and te position of the brick pickup site*/ 
typedef struct Blueprint{ 
  char pos[MaxX][MaxY][MaxZ]; 
  position pickup; 
}; 
 
Blueprint* createBlueprint(){ 
  struct Blueprint* temp = malloc(sizeof(Blueprint)); 
  struct position ps; 
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

Instruction CreateInstruction(int rotation, int x, int y, int z) {
	Instruction newInstruction;
	newInstruction.positions[0] = rotation;
	newInstruction.positions[1] = x;
	newInstruction.positions[2] = y;
	newInstruction.positions[3] = z;

	newInstruction.count = 1; //Change to 0 when doing rotation :3

	return newInstruction;
}

enum brickState
{
	empty, notPlaced, placed
};

Instruction GetInstruction(Blueprint bp, position * bpProgress) {
Instruction inst;

	for (int yAxis = bpProgress->y; yAxis < MaxY; yAxis++)
	{
		for (int zAxis = bpProgress->z; zAxis < MaxZ; zAxis+=2)
		{
			for (int xAxis = bpProgress->x; xAxis < MaxX; xAxis+=2)
			{
				switch (bp.pos[xAxis][yAxis][zAxis])
				{
					case notPlaced:
						switch (bp.pos[xAxis + 2][yAxis][zAxis])
						{
							case notPlaced:
								bp.pos[xAxis][yAxis][zAxis] = placed;
								bp.pos[xAxis + 2][yAxis][zAxis] = placed;
								inst = CreateInstruction();
							break;
							case placed: case empty:
								switch (bp.pos[xAxis - 2][yAxis][zAxis])
								{
									case notPlaced:
										bp.pos[xAxis][yAxis][zAxis] = placed;
										bp.pos[xAxis - 2][yAxis][zAxis] = placed;
										//Place big brick)
									break;
									case placed: case empty:
										switch (bp.pos[xAxis][yAxis+2][zAxis])
										{
											case notPlaced:
												bp.pos[xAxis][yAxis][zAxis] = placed;
												bp.pos[xAxis][yAxis+2][zAxis] = placed;
												//Place big brick)
												break;
											case placed: case empty:
												switch (bp.pos[xAxis][yAxis-2][zAxis])
												{
													case notPlaced:
														bp.pos[xAxis][yAxis][zAxis] = placed;
														bp.pos[xAxis][yAxis-2][zAxis] = placed;
														//Place big brick)
														break;
													case placed: case empty:
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

	return inst;
}