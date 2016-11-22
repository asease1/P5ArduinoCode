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
<<<<<<< HEAD
}
=======
}

void * createInstruction() {

}

enum brickState
{
	empty, notPlaced, placed
};

Instruction GetInstruction(Blueprint bp, position * bpProgress) {
Instruction inst;

	for (int i = bpProgress->x; i < MaxX; i+=2)
	{
		for (int j = bpProgress->z; j < MaxY; j+=2)
		{
			for (int k = bpProgress->y; k < MaxZ; k++)
			{
				switch (bp.pos[i][j][k])
				{
					case notPlaced:
						switch (bp.pos[i + 2][j][k])
						{
							case notPlaced:
								bp.pos[i][j][k] = placed;
								bp.pos[i + 2][j][k] = placed;
								//Place big brick)
							break;
							case placed: case empty:
								switch (bp.pos[i - 2][j][k])
								{
									case notPlaced:
										bp.pos[i][j][k] = placed;
										bp.pos[i - 2][j][k] = placed;
										//Place big brick)
									break;
									case placed: case empty:
										switch (bp.pos[i][j+2][k])
										{
										case notPlaced:
											bp.pos[i][j][k] = placed;
											bp.pos[i][j+2][k] = placed;
											//Place big brick)
											break;
										case placed: case empty:
											switch (bp.pos[i][j-2][k])
											{
												case notPlaced:
													bp.pos[i][j][k] = placed;
													bp.pos[i][j-2][k] = placed;
													//Place big brick)
													break;
												case placed: case empty:
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
>>>>>>> blueprint-to-instruction
