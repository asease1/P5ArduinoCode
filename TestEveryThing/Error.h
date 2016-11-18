#include "Wire.h";

#define ERR_FATAL 0 //generic everything is R.I.P error
#define ERR_WALL 1 //If we recieve a invalid wall
#define ERR_EMPTY_INPUT 2 //empty input stream
#define ERR_BAD_INPUT 3 //Input is nonsensical/wrong

void InitializeErrorComms() {
	Wire.begin(); // wake up I2C bus
				  // set I/O pins to outputs
	Wire.beginTransmission(0x20);
	Wire.write(0x00); // IODIRA register
	Wire.write(0x00); // set all of port A to outputs
	Wire.endTransmission();
	Wire.beginTransmission(0x20);
	Wire.write(0x01); // IODIRB register
	Wire.write(0x00); // set all of port B to outputs
	Wire.endTransmission();
}

void ErrorCode(int error_Code) {
	Wire.beginTransmission(0x20); //Ip of the chip we want to talk to
	Wire.write(0x12); // choice the side of the chip
					  //side A 0x13 and B 0x13 
	Wire.write(error_Code); //turn on the pin with a number from 0-255
	Wire.endTransmission();
}