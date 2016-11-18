/*Contains a single position in 3d space*/
typedef struct position{
  char x;
  char y;
  char z;
};

/*Contains a three-dimensional array of chars and te position of the brick pickup site*/
typedef struct Blueprint{
  char pos[26][3][15];
  position pickup;
};

struct Blupeprint createBlueprint{
  struct Blueprint temp;
  struct position ps;
  char a, b, c;
  /*initializing all values in the array to 0*/
  for(a = 0; a < 26; a++){
    for(b = 0; b < 3; b++){
      for(c = 0; c < 15; c++){
        temp.pos[a][b][c] = 0;
        }
      }
    }
  /*initializing pickup site position to (0, 0, 0)*/
  temp.pickup.x = 0;
  temp.pickup.y = 0;
  temp.pickup.z = 0;
  return bp;
  }
