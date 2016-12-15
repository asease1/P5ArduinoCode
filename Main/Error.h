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
