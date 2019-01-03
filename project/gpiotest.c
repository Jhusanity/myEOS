#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#define BUTTON_PIN 0

int state = 0;

void myInterrupt(void) {
   printf("show state: %d\n", state);
}

int main(void) {
  if (wiringPiSetup () < 0) {
      fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
      return 1;
  }

  if ( wiringPiISR (BUTTON_PIN, INT_EDGE_BOTH, &myInterrupt) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      return 1;
  }

  // display counter value every second.
  while ( 1 ) {
    //printf( "%d\n", state );
    state = digitalRead(BUTTON_PIN);
    delay( 50 ); // wait 1 second
  }

  return 0;
}
