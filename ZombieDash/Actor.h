#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

//********************************
//Actor class
//********************************
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* w, int imageID, double x, double y, int dir, int depth);
	virtual void doSomething() = 0;

	bool blocksMovement() const;
	void unblockMovement();

	virtual double getDistToZombie() const;

	virtual bool isInfectedOrNot() const;
	virtual bool blocksFlame() const;
	virtual bool isDamagedByFlame() const;

	virtual bool triggersOnlyActiveLandmines();

	virtual bool canVomit() const;
	virtual bool canFollowP() const;

	virtual bool canGetInfected() const;
	virtual void setInfected();				//?????

	int randomFromTwoNumbers(int n1, int n2);

	StudentWorld* getWorld() const;
	bool isAlive() const;
	void setDead();
private:
	StudentWorld* world;
	bool alive;
	bool blockMovement;
};
//********************************
//Agent class
//********************************
class Agent : public Actor
{
public:
	Agent(StudentWorld* w, int ID, double x, double y);
	unsigned long whichTick() const;
	void incrementTickCount();
	virtual void doSomething();

	virtual bool canVomit() const;
	virtual bool canGetInfected() const;
	virtual void setInfected();

	bool triggersOnlyActiveLandmines();
	

private:
	unsigned long tickCount;
};
//********************************
//Human class
//********************************
class Human : public Agent
{
public:
	Human(StudentWorld* w, int ID, double x, double y);

	virtual void doSomething();

	void setInfected();
	bool isInfectedOrNot() const;
	bool canGetInfected() const;
	void incrementInfectCount();
	void resetInfectionCount();
	int infectionTotalCount() const;
	void setHealed();

private:
	bool isInfected;
	int infectionTickCount;
};
//********************************
//Penelope class
//********************************
class Penelope : public Human
{
public:
	Penelope(StudentWorld* worldParam, double x, double y);
	void doSomething();
	void incrementFlameThrowerCount();
	void incrementVaccineCount();
	void incrementLandmines();
	
	int flameThrowerCount();
	int vaccinesCount();
	int landminesCount();
private:
	int landmineCount;
	int flamethrowerCount;
	int vaccineCount;
};
//********************************
//Citizen class
//********************************
class Citizen : public Human
{
public:
	Citizen(StudentWorld* worldParam, double x, double y);
	void doSomething();
	double getDistToZombie() const;
private:
	double dist_p;
	double dist_z;
};
//********************************
//Zombie class
//********************************
class Zombie : public Agent
{
public:
	Zombie(StudentWorld* w, double x, double y);
	virtual void doSomething();

	void setNewMovementPlan();
	void resetMovementPlan();
	void decrementMovementPlan();
	int currentMovementPlan() const;

	void moveZombie();
	bool canVomit() const;
private:
	int movementPlan;
};
//********************************
//Dumb Zombie class
//********************************
class DumbZombie : public Zombie
{
public:
	DumbZombie(StudentWorld* w, double x, double y);
	void doSomething();
};
//********************************
//Smart Zombie class
//********************************
class SmartZombie : public Zombie
{
public:
	SmartZombie(StudentWorld* w, double x, double y);
	void doSomething();
	bool canFollowP() const;
private:
	double dist_p;
};
//********************************
//Wall class
//********************************
class Wall : public Actor
{
public:
	Wall(StudentWorld* worldParam, double x, double y);
	void doSomething();
	bool blocksFlame() const;
	bool isDamagedByFlame() const;

};
//********************************
//ActivatingObj class
//********************************
class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* w, int imageID, double x, double y, int dir, int depth);
	virtual void doSomething();

	bool canGetInfected() const;
	virtual bool blocksFlame() const;
	virtual bool isDamagedByFlame() const;
	void makeActive();
	bool isActive() const;

private:
	bool isActiveBool;
};
//********************************
//Exit class
//********************************
class Exit : public ActivatingObject
{
public:
	Exit(StudentWorld* w, double x, double y);
	void doSomething();
	bool blocksFlame() const;
	bool isDamagedByFlame() const;

};
//********************************
//Pit class
//********************************
class Pit : public ActivatingObject 
{
public:
	Pit(StudentWorld* w, double x, double y);
	void doSomething();
	bool isDamagedByFlame() const;
};
//********************************
//Bomb class
//********************************
class LandmineBomb : public ActivatingObject
{
public:
	LandmineBomb(StudentWorld* w, double x, double y);
	void doSomething();
private:
	int safetyTicks;
};
//********************************
//Goodie class
//********************************
class Goodie : public ActivatingObject
{
public:
	Goodie(StudentWorld* w, int imageID, double x, double y, int dir);
	virtual void doSomething();
};
//********************************
//Landmines class
//********************************
class LandmineG : public Goodie
{
public:
	LandmineG(StudentWorld* w, double x, double y);
	void doSomething();
};
//********************************
//GasCan class
//********************************
class GasCan : public Goodie
{
public:
	GasCan(StudentWorld* w, double x, double y);
	void doSomething();
};
//********************************
//Vaccine class
//********************************
class Vaccine : public Goodie
{
public:
	Vaccine(StudentWorld* w, double x, double y);
	void doSomething();
};
//********************************
//Projectile class
//********************************
class Projectile : public ActivatingObject
{
public:
	Projectile(StudentWorld* w, int imageID, double x, double y, int dir);
	virtual void doSomething();
	int returnTickCount() const;
	bool isDamagedByFlame() const;
private:
	int ticCount;
};
//********************************
//Vomit class
//********************************
class Vomit : public Projectile
{
public:
	Vomit(StudentWorld* w, double x, double y, int dir);
	void doSomething();
};
//********************************
//Flaame class
//********************************
class Flame : public Projectile
{
public:
	Flame(StudentWorld* w, double x, double y, int dir);
	void doSomething();
	bool triggersOnlyActiveLandmines();
};

//************************
#endif // ACTOR_H_
