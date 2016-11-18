#include "Wire.h";

#define ERR_FATAL 0 //generic everything is R.I.P error
#define ERR_WALL 1 //If we recieve a invalid wall
#define ERR_EMPTY_INPUT 2 //empty input stream
#define ERR_BAD_INPUT 3 //Input is nonsensical/wrong

void ErrorCode(int error_Code) {
	Wire.beginTransmission(0x20); //Ip of the chip we want to talk to
	Wire.write(0x12); // choice the side of the chip
					  //side A 0x13 and B 0x13 
	Wire.write(error_Code); //turn on the pin with a number from 0-255
	Wire.endTransmission();
}