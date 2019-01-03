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
#define NUMTHREADS 4
/* 
 * 1. get gpio
 * 2. send to net
 * 3. get camera
 * 4. score
 */

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
void toilet2lockChange(void){
    // save the inversed locknum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", -toiletlockstate2+1, toiletoccupied2);
}
void toilet2occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", toiletlockstate2, -toiletoccupied2+1);
}

void *GetGPIO()
{
    printf("Hello! I am thread GetGPIO\n");
    while(1){
        toiletlockstate1 = digitalRead(TOILET1LOCKPIN);
        toiletoccupied1 = digitalRead(TOILET1OCCUPYPIN);
        toiletlockstate2 = digitalRead(TOILET2LOCKPIN);
        toiletoccupied2 = digitalRead(TOILET2OCCUPYPIN);
        delay( 50 );
    }
    pthread_exit(NULL);
}

void *SendToNet()
{
    printf("Hello! I am thread SendToNet\n");
    while(1){
        //printf("Send to Net!!!\n");
        delay(5);
    }
    pthread_exit(NULL);
}

void *GetCam()
{
    printf("Hello! I am thread GetCam\n");
    while(1){
       // printf("eat photo\n");
        delay(1);
    }   
    pthread_exit(NULL);
}

void *Score()
{
    printf("Hello! I am thread Score\n");
    pthread_exit(NULL);
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
    if ( wiringPiISR (TOILET2LOCKPIN, INT_EDGE_BOTH, &toilet2lockChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET2OCCUPYPIN, INT_EDGE_BOTH, &toilet2occupyChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }         

    pthread_t threads[NUMTHREADS];
    int rc;

    for(long t=0;t<NUMTHREADS;t++){
        printf("Main: creating thread %ld\n", t);

        if (t == 0)
            rc = pthread_create(&threads[t], NULL, GetGPIO, NULL);
        else if (t == 1)
            rc = pthread_create(&threads[t], NULL, SendToNet, NULL);
        else if (t == 2)
            rc = pthread_create(&threads[t], NULL, GetCam, NULL);
        else if (t == 3)
            rc = pthread_create(&threads[t], NULL, Score, NULL);

        if(rc){
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }   

    for(int t=0;t<NUMTHREADS;t++){
        rc = pthread_join(threads[t], NULL);
        if(rc){
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        printf("Main: join with thread %d\n", t);
    }   
    printf("Main: program completed Exiting. \n");

    return 0;
}
