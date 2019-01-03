#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TOILET1LOCKPIN 0
#define TOILET1OCCUPYPIN 1
#define TOILET2LOCKPIN 2
#define TOILET2OCCUPYPIN 3


int toiletlockstate1 = 0;
int toiletoccupied1 = 0;
int toiletlockstate2 = 0;
int toiletoccupied2 = 0;
void toilet1lockChange(void){
    // save the inversed locknum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", -toiletlockstate1+1, toiletoccupied1);
}
void toilet1occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", toiletlockstate1, -toiletoccupied1+1);
}

int main(int argc, char *argv[])
{
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET1LOCKPIN, INT_EDGE_BOTH, &toilet1lockChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET1OCCUPYPIN, INT_EDGE_BOTH, &toilet1occupyChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }

    while(1){
        toiletlockstate1 = digitalRead(TOILET1LOCKPIN);
        toiletoccupied1 = digitalRead(TOILET1OCCUPYPIN);
        toiletlockstate2 = digitalRead(TOILET2LOCKPIN);
        toiletoccupied2 = digitalRead(TOILET2OCCUPYPIN);
        delay( 50 );
    }

    return 0;
}
