#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
  printf ("Raspberry Pi wiringPi test program\n") ;

  if (wiringPiSetupGpio() == -1)
    exit (1) ;

  pinMode(18,PWM_OUTPUT);
  pwmSetClock(2);
  pwmSetRange (10) ;
  pwmWrite (18, 5);

for (;;) delay (1000) ;
}
