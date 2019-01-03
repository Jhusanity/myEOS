#include <pthread.h>
//#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "Toilet.h"

struct Restroom myrestroom={17251,2,0};

int main()
{
	printf("ID: %d\nscore: %d\n", myrestroom.ID, myrestroom.score);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[0].locked, myrestroom.toilets[0].occupied);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[1].locked, myrestroom.toilets[1].occupied);
	
	//score
	double x[2];
	for(int i=0;i<2;i++){
		x[i] = (-myrestroom.toilets[i].locked+1)*(1-myrestroom.toilets[i].occupied*0.5);
	}
	double k = 2 + myrestroom.waitingpeople - x[0] - x[1];
	printf("%f\n",k);
	myrestroom.score = round(100 * pow(0.9,k));
	printf("\nID: %d\nscore: %d\n", myrestroom.ID, myrestroom.score);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[0].locked, myrestroom.toilets[0].occupied);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[1].locked, myrestroom.toilets[1].occupied);
	
	myrestroom.waitingpeople = 0;

	myrestroom.toilets[0].locked = 0;
	myrestroom.toilets[0].occupied = 1;

	myrestroom.toilets[1].locked = 1;
	myrestroom.toilets[1].occupied = 1;

	//score
	x[2];
	for(int i=0;i<2;i++){
		x[i] = (-myrestroom.toilets[i].locked+1)*(1-myrestroom.toilets[i].occupied*0.5);
	}
	k = 2 + myrestroom.waitingpeople - x[0] - x[1];
	printf("%f\n",k);
	myrestroom.score = round(100 * pow(0.9,k));
	printf("\nID: %d\nscore: %d\n", myrestroom.ID, myrestroom.score);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[0].locked, myrestroom.toilets[0].occupied);
	printf("locked: %d\noccupied: %d\n", myrestroom.toilets[1].locked, myrestroom.toilets[1].occupied);
	return 0;
}