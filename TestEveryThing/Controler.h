//Control that defines motor and a controler that contains the motor 

//Chanel is the current motor input Interupts
enum Chanels {motorY, motorX, motorZ, motorRotation};
enum motorstates {forward, backward, hold};


struct Motor{
    volatile int pos;
    int maxPos;
    int minPos;
    int pin1;
    int pin2;
    motorstates state;
    volatile int sig1;
    volatile int sig2;
};

Motor CreateMotor(int maxPos, int pin1, int pin2){
  Motor newMotor;
  newMotor.pos = 0;
  newMotor.maxPos = maxPos;
  newMotor.minPos = minPos;
  newMotor.pin1 = pin1;
  newMotor.pin2 = pin2;
  newMotor.state = hold;
  //Can check the sig pins in here if you chance chanel to get a more precise position.
  newMotor.sig1 = 0;
  newMotor.sig2 = 0;

  return newMotor;
}

