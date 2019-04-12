#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//********************************
//Actor class
//********************************
Actor::Actor(StudentWorld* w, int ID, double x, double y, int dir, int depth)
	:GraphObject(ID, x, y, dir, depth), alive(true), blockMovement(true)
{
	world = w;
}

StudentWorld* Actor::getWorld() const
{
	return world;
}

bool Actor::isAlive() const
{
	return alive;
}

void Actor::setDead()
{
	alive = false;
}

double Actor::getDistToZombie() const
{
	return 0.0;
}

bool Actor::blocksMovement() const
{
	return blockMovement;
}

bool Actor::canFollowP() const
{
	return false;
}

void Actor::unblockMovement()
{
	blockMovement = false;
}

bool Actor::isInfectedOrNot() const
{
	return false;
}

bool Actor::triggersOnlyActiveLandmines()
{
	return false;
}

bool Actor::blocksFlame() const
{
	return false;
}

bool Actor::isDamagedByFlame() const
{
	return true;
}

bool Actor::canGetInfected() const
{
	return false;
}

bool Actor::canVomit() const
{
	return false;
}

void Actor::setInfected()
{}

int Actor::randomFromTwoNumbers(int n1, int n2)
{
	int r;
	r = rand();
	if (r % 2 == 0)
		return n1;  // use n1;
	else
		return n2;  //use n2 
}


//********************************
//Agent class
//********************************

Agent::Agent(StudentWorld* w, int ID, double x, double y)
	:Actor(w, ID, x, y, 0, 0), tickCount(0)
{}

void Agent::doSomething()
{
	incrementTickCount();
}

unsigned long Agent::whichTick() const
{
	return tickCount;
}

void Agent::incrementTickCount()
{
	tickCount++;
}

bool Agent::triggersOnlyActiveLandmines()
{
	return true;
}

bool Agent::canGetInfected() const
{
	return Actor::canGetInfected();
}

bool Agent::canVomit() const
{
	return Actor::canVomit();
}

void Agent::setInfected()
{}


//********************************
//Human class
//********************************

Human::Human(StudentWorld* w, int ID, double x, double y)
	: Agent(w, ID, x, y), isInfected(false), infectionTickCount(0)
{}

void Human::setInfected()
{
	isInfected = true;
}

bool Human::isInfectedOrNot() const
{
	return isInfected;
}

void Human::incrementInfectCount()
{
	infectionTickCount++;
}

void Human::resetInfectionCount()
{
	infectionTickCount = 0;
}

int Human::infectionTotalCount() const
{
	return infectionTickCount;
}

bool Human::canGetInfected() const
{
	return true;
}

void Human::doSomething()
{
	if (isInfectedOrNot())
		incrementInfectCount();
}

void Human::setHealed()
{
	resetInfectionCount();
	isInfected = false;
}

//********************************
//Penelope class
//********************************

Penelope::Penelope(StudentWorld* worldParam, double x, double y)
	: Human(worldParam, IID_PLAYER, x, y)
{
	landmineCount = 0;
	flamethrowerCount = 0;
	vaccineCount = 0;
}

void Penelope::incrementFlameThrowerCount()
{
	flamethrowerCount += 5;
}

void Penelope::incrementVaccineCount()
{
	vaccineCount++;
}

void Penelope::incrementLandmines()
{
	landmineCount += 2;
}

int Penelope::flameThrowerCount()
{
	return flamethrowerCount;
}

int Penelope::vaccinesCount()
{
	return vaccineCount;
}

int Penelope::landminesCount()
{
	return landmineCount;
}

void Penelope::doSomething()
{
	if (!isAlive())
		return;
	Human::doSomething();
	if (infectionTotalCount() == 500) {
		setDead();
		getWorld()->playSound(SOUND_PLAYER_DIE);
		return;
	}
	int keyPressed;
	if (getWorld()->getKey(keyPressed))
	{
		// user hit a key during this tick!
		switch (keyPressed)
		{
		case KEY_PRESS_SPACE:
			if (flamethrowerCount > 0) {
				flamethrowerCount--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				for (int i = 1; i < 4; i++)
				{
					if (getDirection() == GraphObject::left) {
						if (!getWorld()->isFlameBlocked(getX() - i * SPRITE_WIDTH, getY()))
							getWorld()->addActor(new Flame(getWorld(), getX() - i * SPRITE_WIDTH, getY(), GraphObject::left));
						else
							break;
					}
					else if (getDirection() == GraphObject::right) {
						if (!getWorld()->isFlameBlocked(getX() + i * SPRITE_WIDTH, getY()))
							getWorld()->addActor(new Flame(getWorld(), getX() + i * SPRITE_WIDTH, getY(), GraphObject::right));
						else
							break;
					}
					else if (getDirection() == GraphObject::up) {
						if (!getWorld()->isFlameBlocked(getX(), getY() + i * SPRITE_HEIGHT))
							getWorld()->addActor(new Flame(getWorld(), getX(), getY() + i * SPRITE_HEIGHT, GraphObject::up));
						else
							break;
					}
					else {
						if (!getWorld()->isFlameBlocked(getX(), getY() - i * SPRITE_HEIGHT))
							getWorld()->addActor(new Flame(getWorld(), getX(), getY() - i * SPRITE_HEIGHT, GraphObject::up));
						else
							break;
					}
				}
			}
			break;
		case KEY_PRESS_TAB:
			if (landmineCount > 0) {
				landmineCount--;
				getWorld()->addActor(new LandmineBomb(getWorld(), getX(), getY()));
			}
			break;
		case KEY_PRESS_ENTER:
			if (vaccineCount > 0) {
				vaccineCount--;
				setHealed();
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(GraphObject::left);
			if (!getWorld()->isBlocked(GraphObject::left, this, 4))
				getWorld()->moveTheActor(this, getDirection(), 4);
			break;
		case KEY_PRESS_RIGHT:
			setDirection(GraphObject::right);
			if (!getWorld()->isBlocked(GraphObject::right, this, 4))
				getWorld()->moveTheActor(this, getDirection(), 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(GraphObject::down);
			if (!getWorld()->isBlocked(GraphObject::down, this, 4))
				getWorld()->moveTheActor(this, getDirection(), 4);
			break;
		case KEY_PRESS_UP:
			setDirection(GraphObject::up);
			if (!getWorld()->isBlocked(GraphObject::up, this, 4))
				getWorld()->moveTheActor(this, getDirection(), 4);
			break;
		}
	}
}


//********************************
//Citizen class
//********************************

Citizen::Citizen(StudentWorld* worldParam, double x, double y)
	: Human(worldParam, IID_CITIZEN, x, y)
{}

void Citizen::doSomething()
{
	if (!isAlive())
		return;

	Agent::doSomething();	
	Human::doSomething();
											//increments tick count
	if (whichTick() % 2 == 0)
		return;

	if (infectionTotalCount() >= 500)
	{
		setDead();
		getWorld()->playSound(SOUND_ZOMBIE_BORN);
		getWorld()->increaseScore(-1000);
		bool isDumb = getWorld()->oneInSeven();
		if (isDumb)
			getWorld()->addActor(new DumbZombie(getWorld(), getX(), getY()));
		else
			getWorld()->addActor(new SmartZombie(getWorld(), getX(), getY()));
		return;
	}

	int dirToPene = 0;
	dist_p = getWorld()->distanceFromACitizenToPen(this, dirToPene);
	int dirToZombie = 0;
	dist_z = getWorld()->distanceFromACitizenToAZombie(this, dirToZombie);

	if (dist_p < dist_z && dist_p <= 80 && !getWorld()->isBlocked(dirToPene, this, 2)) {
		setDirection(dirToPene);
		getWorld()->moveTheActor(this, dirToPene, 2);
		//move in  that respective direction
	}
	else if (dist_z <= 80 && !getWorld()->isBlocked(dirToZombie, this, 2)) {
		setDirection(dirToZombie);
		getWorld()->moveTheActor(this, dirToZombie, 2);
		//move in  that respective direction
	}
	else
		return;

}

double Citizen::getDistToZombie() const
{
	return dist_z;
}

//********************************
//Zombie class
//********************************

Zombie::Zombie(StudentWorld* w, double x, double y)
	: Agent(w, IID_ZOMBIE, x, y), movementPlan(0)
{}

void Zombie::doSomething()
{
	Agent::doSomething();
}

bool Zombie::canVomit() const
{
	return true;
}

void Zombie::setNewMovementPlan()
{
	movementPlan = randInt(3, 10);
	setDirection(randInt(0, 3) * 90);
}

void Zombie::resetMovementPlan()
{
	movementPlan = 0;
}

void Zombie::decrementMovementPlan()
{
	movementPlan--;
}

int Zombie::currentMovementPlan() const
{
	return movementPlan;
}

void Zombie::moveZombie()
{
	switch (getDirection())
	{
	case GraphObject::left:
		if (getWorld()->isZombieVomitTriggerAt(getX() - SPRITE_WIDTH, getY()))
			getWorld()->spitVomit(getDirection(), this);

		if (!getWorld()->isBlocked(GraphObject::left, this, 1)) {
			moveTo(getX() - 1, getY());
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::right:
		if (getWorld()->isZombieVomitTriggerAt(getX() + SPRITE_WIDTH, getY()))
			getWorld()->spitVomit(getDirection(), this);

		if (!getWorld()->isBlocked(GraphObject::right, this, 1)) {
			moveTo(getX() + 1, getY());
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::down:
		if (getWorld()->isZombieVomitTriggerAt(getX(), getY() - SPRITE_HEIGHT))
			getWorld()->spitVomit(getDirection(), this);

		if (!getWorld()->isBlocked(GraphObject::down, this, 1)) {
			moveTo(getX(), getY() - 1);
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::up:
		if (getWorld()->isZombieVomitTriggerAt(getX(), getY() + SPRITE_HEIGHT))
			getWorld()->spitVomit(getDirection(), this);

		if (!getWorld()->isBlocked(GraphObject::up, this, 1)) {
			moveTo(getX(), getY() + 1);
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	}
}

//********************************
//DumbZombie class
//********************************

DumbZombie::DumbZombie(StudentWorld* w, double x, double y)
	:Zombie(w, x, y)
{}

void DumbZombie::doSomething()
{
	if (!isAlive())
		return;

	Zombie::doSomething();		//increments tickCount
	if (whichTick() % 2 == 0)
		return;

	if (currentMovementPlan() == 0)
		setNewMovementPlan();

	moveZombie();
}

//********************************
//SmartZombie class
//********************************

SmartZombie::SmartZombie(StudentWorld* w, double x, double y)
	:Zombie(w, x, y)
{}

bool SmartZombie::canFollowP() const
{
	return true;
}

void SmartZombie::doSomething()
{
	if (!isAlive())
		return;

	Zombie::doSomething();		//increments tickCount
	if (whichTick() % 2 == 0)
		return;

	if (currentMovementPlan() == 0)
		setNewMovementPlan();

	int dirToPene = 0;
	Actor* target;
	target = getWorld()->getClosestVictim(this, dirToPene);
	dist_p = getWorld()->euclideanDis(this, target);

	switch (getDirection())
	{
	case GraphObject::left:
		if (getWorld()->isZombieVomitTriggerAt(getX() - SPRITE_WIDTH, getY()))
			getWorld()->spitVomit(getDirection(), this);
		if (dist_p <= 80 && !getWorld()->isBlocked(dirToPene, this, 1)) {
			setDirection(dirToPene);
			getWorld()->moveTheActor(this, dirToPene, 1);
			//move in  that respective direction
		}
		else if (!getWorld()->isBlocked(GraphObject::left, this, 1)) {
			moveTo(getX() - 1, getY());
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::right:
		if (getWorld()->isZombieVomitTriggerAt(getX() + SPRITE_WIDTH, getY()))
			getWorld()->spitVomit(getDirection(), this);

		if (dist_p <= 80 && !getWorld()->isBlocked(dirToPene, this, 1)) {
			setDirection(dirToPene);
			getWorld()->moveTheActor(this, dirToPene, 1);
			//move in  that respective direction
		}
		else if (!getWorld()->isBlocked(GraphObject::right, this, 1)) {
			moveTo(getX() + 1, getY());
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::down:
		if (getWorld()->isZombieVomitTriggerAt(getX(), getY() - SPRITE_HEIGHT))
			getWorld()->spitVomit(getDirection(), this);

		if (dist_p <= 80 && !getWorld()->isBlocked(dirToPene, this, 1)) {
			setDirection(dirToPene);
			getWorld()->moveTheActor(this, dirToPene, 1);
			//move in  that respective direction
		}
		else if (!getWorld()->isBlocked(GraphObject::down, this, 1)) {
			moveTo(getX(), getY() - 1);
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	case GraphObject::up:
		if (getWorld()->isZombieVomitTriggerAt(getX(), getY() + SPRITE_HEIGHT))
			getWorld()->spitVomit(getDirection(), this);

		if (dist_p <= 80 && !getWorld()->isBlocked(dirToPene, this, 1)) {
			setDirection(dirToPene);
			getWorld()->moveTheActor(this, dirToPene, 1);
			//move in  that respective direction
		}
		else if (!getWorld()->isBlocked(GraphObject::up, this, 1)) {
			moveTo(getX(), getY() + 1);
			decrementMovementPlan();
		}
		else
			resetMovementPlan();
		break;
	}
	
}


//********************************
//Wall class
//********************************

Wall::Wall(StudentWorld* worldParam, double x, double y)
	:Actor(worldParam, IID_WALL, x, y, 0, 0)
{}

void Wall::doSomething()
{}

bool Wall::blocksFlame() const
{
	return true;
}

bool Wall::isDamagedByFlame() const
{
	return false;
}

//********************************
//ActivatingObject class
//********************************

ActivatingObject::ActivatingObject(StudentWorld* w, int imageID, double x, double y, int dir, int depth)
	:Actor(w, imageID, x, y, dir, depth), isActiveBool(false)
{
	unblockMovement();
}

void ActivatingObject::makeActive()
{
	isActiveBool = true;
}

bool ActivatingObject::isActive() const
{
	return isActiveBool;
}

void ActivatingObject::doSomething()
{}

bool ActivatingObject::canGetInfected() const
{
	return Actor::canGetInfected();
}

bool ActivatingObject::blocksFlame() const
{
	return false;
}

bool ActivatingObject::isDamagedByFlame() const
{
	return true;
}

//********************************
//Pit class
//********************************
Pit::Pit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_PIT, x, y, 0, 0)
{}

void Pit::doSomething()
{
	getWorld()->swallowAllNearbyObjects(this);
}

bool Pit::isDamagedByFlame() const
{
	return false;
}

//********************************
//LandMineBomb class
//********************************

LandmineBomb::LandmineBomb(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_LANDMINE, x, y, 0, 1), safetyTicks(30)
{}

void LandmineBomb::doSomething()
{
	if (!isAlive())
		return;
	if (!isActive()) {
		safetyTicks--;
		if (safetyTicks <= 0)
			makeActive();
		return;
	}
	else
		getWorld()->explodeAllNearbyObjects(this);
}

//********************************
//Exit class
//********************************

Exit::Exit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_EXIT, x, y, 0, 1)
{}

void Exit::doSomething()
{
	if (getWorld()->overlapsWithCitizen(this))
	{
		getWorld()->increaseScore(500);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
	}

	if (getWorld()->areAllCitizensGone() && getWorld()->distanceFromAnActorToPen(this) <= 10)
		getWorld()->setLevelToFinished();
	else
		return;
}

bool Exit::blocksFlame() const
{
	return true;
}

bool Exit::isDamagedByFlame() const
{
	return false;
}

//********************************
//Goodie class
//********************************

Goodie::Goodie(StudentWorld* w, int imageID, double x, double y, int dir)
	:ActivatingObject(w, imageID, x, y, dir, 1)
{}

void Goodie::doSomething()
{}

//********************************
//Vaccine class
//********************************

Vaccine::Vaccine(StudentWorld* w, double x, double y)
	:Goodie(w, IID_VACCINE_GOODIE, x, y, 0)
{}

void Vaccine::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->distanceFromAnActorToPen(this) <= 10) {
		getWorld()->increaseScore(50);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->incrementVaccineCount();
	}
}

//********************************
//LandMine class
//********************************

LandmineG::LandmineG(StudentWorld* w, double x, double y)
	: Goodie(w, IID_LANDMINE_GOODIE, x, y, 0)
{}

void LandmineG::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->distanceFromAnActorToPen(this) <= 10) {
		getWorld()->increaseScore(50);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->incrementLandmines();
	}
}

//********************************
//GasCan class
//********************************

GasCan::GasCan(StudentWorld* w, double x, double y)
	:Goodie(w, IID_GAS_CAN_GOODIE, x, y, 0)
{}

void GasCan::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->distanceFromAnActorToPen(this) <= 10) {
		getWorld()->increaseScore(50);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->incrementFlameThrowerCount();
	}
}

//********************************
//Projectile class
//********************************

Projectile::Projectile(StudentWorld* w, int imageID, double x, double y, int dir)
	: ActivatingObject(w, imageID, x, y, dir, 0), ticCount(0)
{}

void Projectile::doSomething()
{
	ticCount++;
}

int Projectile::returnTickCount() const
{
	return ticCount;
}

bool Projectile::isDamagedByFlame() const
{
	return false;
}


//********************************
//Vomit class
//********************************

Vomit::Vomit(StudentWorld* w, double x, double y, int dir)
	:Projectile(w, IID_VOMIT, x, y, dir)
{}

void Vomit::doSomething()
{
	if (!isAlive())
		return;

	Projectile::doSomething();

	if (returnTickCount() >= 2) {
		setDead();
		return;
	}

	getWorld()->vomitInfectingAllNearbyObjects(this);
}

//********************************
//Flame class
//********************************

Flame::Flame(StudentWorld* w, double x, double y, int dir)
	:Projectile(w, IID_FLAME, x, y, dir)
{}

void Flame::doSomething()
{
	if (!isAlive())
		return;

	Projectile::doSomething();

	if (returnTickCount() >= 2) {
		setDead();
		return;
	}

	getWorld()->damageAllNearbyObjects(this);

}

bool Flame::triggersOnlyActiveLandmines()
{
	return true;
}
