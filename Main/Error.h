#include "Wire.h"

#define ERR_FATAL 0 //generic error code
#define ERR_WALL 1 //Error code for wrong wall specifications
#define ERR_EMPTY_INPUT 2 //empty input / no input at all
#define ERR_BAD_INPUT 3 //Input is nonsensical/wrong

void ErrorCode(int error_Code) {
	Wire.beginTransmission(0x20); 
	Wire.write(error_Code);
	Serial.print("ERROR ENCOUNTERED: ");
	Serial.println(error_Code);
	Wire.endTransmission();
}

void InitializeErrorComms() {
	Wire.begin();           // Wake up I2C bus
							// Set I/O pins to outputs
	Wire.beginTransmission(0x20);
	Wire.write(0x00);       // IODIRA register
	Wire.write(0x00);       // Set all of port A to outputs
	Wire.endTransmission();
	Wire.beginTransmission(0x20);
	Wire.write(0x01);       // IODIRB register
	Wire.write(0x00);       // Set all of port B to outputs
	Wire.endTransmission();
}