/*Defined values. Remember they're used in an array, so -1 for actual values!*/
#define MaxX = 26;
#define MaxY = 3;
#define MaxZ = 15;

/*Contains a single position in 3d space*/
typedef struct position{
  char x;
  char y;
  char z;
};

/*Contains a three-dimensional array of chars and te position of the brick pickup site*/
typedef struct Blueprint{
  char pos[MaxX][MaxY][MaxZ];
  position pickup;
};

struct Blupeprint createBlueprint{
  struct Blueprint temp;
  struct position ps;
  char a, b, c;
  /*initializing all values in the array to 0*/
  for(a = 0; a < MaxX; a++){
    for(b = 0; b < MaxY; b++){
      for(c = 0; c < MaxZ; c++){
        temp.pos[a][b][c] = 0;
        }
      }
    }
  /*initializing pickup site position to (0, 0, 0)*/
  temp.pickup.x = 0;
  temp.pickup.y = 0;
  temp.pickup.z = 0;
  return temp;
  }
