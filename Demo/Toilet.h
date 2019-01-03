#ifndef TOILET_H_
#define TOILET_H_

struct Toilet {
	int locked;
	int occupied;
};

struct Restroom{
	int ID;
	int score;
	int waitingpeople;
	struct Toilet toilets[2];
};

#endif // TOILET_H_