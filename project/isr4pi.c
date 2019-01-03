/*
isr4pi.c
D. Thiebaut
based on isr.c from the WiringPi library, authored by Gordon Henderson
https://github.com/WiringPi/WiringPi/blob/master/examples/isr.c

Compile as follows:

    gcc -o isr4pi isr4pi.c -lwiringPi

Run as follows:

    sudo ./isr4pi

 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

// Use GPIO Pin 17, which is GPIO 0 for wiringPi library
#define RISING_PIN 0
#define FALLING_PIN 1
int pressflag = 0;
int releaseflag = 0;
int state = 0;
// -------------------------------------------------------------------------
// ISR
void myBothInterrupt(void){
   state = -state + 1;
   printf("k\n");
}/*
void myPressInterrupt(void) {
   pressflag = 1; 
}

void myReleaseInterrupt(void) {
   releaseflag = 1; 
}*/

// -------------------------------------------------------------------------
// main
int main(void) {
  // sets up the wiringPi library
  if (wiringPiSetup () < 0) {
      fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
      return 1;
  }
/*
  // Attach myReleaseInterrupt() to GPIO 0 with falling edge
  if ( wiringPiISR (FALLING_PIN, INT_EDGE_FALLING, &myReleaseInterrupt) < 0 ) {
      fprintf (stderr, "Unable to setup falling ISR: %s\n", strerror (errno));
      return 1;
  }

  // Attach myPressInterrupt() to GPIO 1 with rising edge
  if ( wiringPiISR (RISING_PIN, INT_EDGE_RISING, &myPressInterrupt) < 0 ) {
      fprintf (stderr, "Unable to setup rising ISR: %s\n", strerror (errno));
      return 1;
  }
*/
  if ( wiringPiISR (0, INT_EDGE_BOTH, &myBothInterrupt) < 0 ) {
      fprintf (stderr, "Unable to setup rising ISR: %s\n", strerror (errno));
      return 1;
  }
  while ( 1 ) {/*
    while( pressflag == 1 ){
       printf("pressed\n");
       pressflag = 0;
    } 
    while( releaseflag == 1 ){
       printf("released\n");
       releaseflag = 0;
    }*/
    printf("state: %d\n",state);	  
    delay( 1000 ); // wait 1 second
  }

  return 0;
}
