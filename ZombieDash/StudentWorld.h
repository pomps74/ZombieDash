#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(string assetPath);
	~StudentWorld();

    int init();
    int move();
    void cleanUp();

	Penelope* getPlayer() const;

	bool isBlocked(Direction x, Actor* currentActor, int pixel);
	void moveTheActor(Actor* doingMoving, Direction dir, int pixel);

	bool areAllCitizensGone();

	double euclideanDis(Actor* x, Actor* y);
	double euclideanDisOfPts(double x1, double y1, double x2, double y2);

	double distanceFromAnActorToPen(Actor* current);
	double distanceFromACitizenToPen(Actor* citizen, Direction& dir);
	double distanceFromACitizenToAZombie(Actor* citizen, Direction& dir);

	Actor* getClosestVictim(Actor* citizen, Direction& dir);

	double calcDestX(int dir, double currentX, int pixel);
	double calcDestY(int dir, double currentY, int pixel);

	void vomitInfectingAllNearbyObjects(Actor* vomitActor);
	void damageAllNearbyObjects(Actor* flameActor);
	void swallowAllNearbyObjects(Actor* pitActor);
	void explodeAllNearbyObjects(Actor* bombActor);

	bool overlapsWithCitizen(Actor* exit);

	bool isZombieVomitTriggerAt(double x, double y);
	void spitVomit(Direction dir, Actor* zombie);

	void addActor(Actor* a);

	bool isFlameBlocked(double x, double y);

	bool oneInThree();
	bool oneInSeven();

	double returnMax(double a, double b);			//RETURNS THE MAXIMUM

	int gameCodeReturn();
	void setLevelToFinished();

private:
	Penelope *p;
	vector<Actor*> obj123;
	bool finishedLevel = false;
};


#endif // STUDENTWORLD_H_
