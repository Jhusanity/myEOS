#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "Toilet.h"

#define TOILET0LOCKPIN 0
#define TOILET0OCCUPYPIN 1
#define TOILET1LOCKPIN 2
#define TOILET1OCCUPYPIN 3
#define NUMTHREADS 4
/* 
 * 1. get gpio
 * 2. send to net
 * 3. get camera
 * 4. score
 */

struct Restroom myrestroom={103,0,0};// ID, score, waitingpeople
int scoring = 0;

void toilet0lockChange(void){
    // save the inversed locknum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", -myrestroom.toilets[0].locked+1, myrestroom.toilets[0].occupied);
    scoring = 1;
}
void toilet0occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 1:\nlocked: %d\noccupied: %d\n", myrestroom.toilets[0].locked, -myrestroom.toilets[0].occupied+1);
    scoring = 1;
}
void toilet1lockChange(void){
    // save the inversed locknum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", -myrestroom.toilets[1].locked+1, myrestroom.toilets[1].occupied);
    scoring = 1;
}
void toilet1occupyChange(void){
    // save the inversed occupynum
    printf("Toilet 2:\nlocked: %d\noccupied: %d\n", myrestroom.toilets[1].locked, -myrestroom.toilets[1].occupied+1);
    scoring = 1;
}

void *GetGPIO()
{
    printf("Hello! I am thread GetGPIO\n");
    while(1){
        myrestroom.toilets[0].locked = digitalRead(TOILET0LOCKPIN);
        myrestroom.toilets[0].occupied = digitalRead(TOILET0OCCUPYPIN);
        myrestroom.toilets[1].locked = digitalRead(TOILET1LOCKPIN);
        myrestroom.toilets[1].occupied = digitalRead(TOILET1OCCUPYPIN);
        delay( 50 );
    }
    pthread_exit(NULL);
}

void *SendToNet()
{
    printf("Hello! I am thread SendToNet\n");
    while(1){
        printf("Send to Net!!!\n");
        delay(5000);
    }
    pthread_exit(NULL);
}

void *GetCam()
{
    printf("Hello! I am thread GetCam\n");
    while(1){
        printf("eat photo\n");
        delay(1000);
    }   
    pthread_exit(NULL);
}

void *Score()
{
    printf("Hello! I am thread Score\n");
    double x[2];
    double k;
    while(1){
        if(scoring == 1){
            for(int i=0;i<2;i++){
                x[i] = (-myrestroom.toilets[i].locked+1)*(1-myrestroom.toilets[i].occupied*0.5);
            }  
            k = 2 + myrestroom.waitingpeople - x[0] - x[1];
            myrestroom.score = round(100 * pow(0.9,k));
            printf("New cosre: %d\n",myrestroom.score);
            scoring = 0;
        }
        delay( 50 );
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }

    if ( wiringPiISR (TOILET0LOCKPIN, INT_EDGE_BOTH, &toilet0lockChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }
    if ( wiringPiISR (TOILET0OCCUPYPIN, INT_EDGE_BOTH, &toilet0occupyChange) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
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