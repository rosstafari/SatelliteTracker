#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 100

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 5, 6, 7, 8);

// the previous reading from the analog input
int previous = 0;
int val = 1;

void setup()
{
  // set the speed of the motor to n RPMs
  stepper.setSpeed(10);
}

void loop()
{
  
  delay(1000);
  stepper.step(150);

}
