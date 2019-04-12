#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
	obj123.clear();
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	Level lev(assetPath());

	string levelFile;

	std::stringstream sstm;
	sstm << "level0" << getLevel() << ".txt";
	levelFile = sstm.str();

	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level " << levelFile << endl;
		for (int row = 0; row < LEVEL_HEIGHT; row++)
		{
			for (int col = 0; col < LEVEL_WIDTH; col++)
			{
				Level::MazeEntry ge = lev.getContentsOf(row, col); // level_x=5, level_y=10
				switch (ge) // so x=80 and y=160
				{
				case Level::empty:
					break;
				case Level::gas_can_goodie:
					obj123.push_back(new GasCan(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::vaccine_goodie:
					obj123.push_back(new Vaccine(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::landmine_goodie:
					obj123.push_back(new LandmineG(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::pit:
					obj123.push_back(new Pit(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::player:
					p = new Penelope(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH);
					obj123.push_back(p);
					break;
				case Level::wall:
					obj123.push_back(new Wall(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::dumb_zombie:
					obj123.push_back(new DumbZombie(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::smart_zombie:
					obj123.push_back(new SmartZombie(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::exit:
					obj123.push_back(new Exit(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				case Level::citizen:
					obj123.push_back(new Citizen(this, row * SPRITE_HEIGHT, col * SPRITE_WIDTH));
					break;
				}
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::gameCodeReturn()
{
	if (finishedLevel) {
		playSound(SOUND_LEVEL_FINISHED);
		finishedLevel = false;
		return GWSTATUS_FINISHED_LEVEL;
	}
	else
		return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setLevelToFinished()
{
	finishedLevel = true;
}

bool StudentWorld::areAllCitizensGone()
{
	for (int i = 0; i < obj123.size(); i++) {
		if (obj123[i] != p && obj123[i]->canGetInfected())
			return false;
	}
	return true;
}

int StudentWorld::move()
{
	string stre;

	std::stringstream sstm;
	sstm << "Score: ";
	if (getScore() < 0)
		sstm << "-" << setfill('0') << setw(5) << getScore() * -1;
	else
		sstm << setfill('0') << setw(6) << getScore();
	sstm << "  Level: " << getLevel() <<
		"  Lives: " << getLives() <<
		"  Vacc: " << p->vaccinesCount() <<
		"  Flames: " << p->flameThrowerCount() <<
		"  Mines: " << p->landminesCount() <<
		"  Infected: " << p->infectionTotalCount();
	stre = sstm.str();

	setGameStatText(stre);


	p->doSomething();
	if (!p->isAlive()) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	for (int i = 0; i < obj123.size() && finishedLevel == false; i++) {
		if (obj123[i] == p)
			continue;
		if (!obj123[i]->isAlive()) {
			delete obj123[i];
			obj123.erase(obj123.begin() + i);
			i--;
			continue;
		}
		obj123[i]->doSomething();
	}
	return gameCodeReturn();
}

Penelope* StudentWorld::getPlayer() const
{
	return p;
}

void StudentWorld::moveTheActor(Actor* doingMoving, Direction dir, int pixel)
{
	switch (dir)
	{
	case GraphObject::left:
		doingMoving->moveTo(doingMoving->getX() - pixel, doingMoving->getY());
		break;
	case GraphObject::right:
		doingMoving->moveTo(doingMoving->getX() + pixel, doingMoving->getY());
		break;
	case GraphObject::down:
		doingMoving->moveTo(doingMoving->getX(), doingMoving->getY() - pixel);
		break;
	case GraphObject::up:
		doingMoving->moveTo(doingMoving->getX(), doingMoving->getY() + pixel);
		break;
	}
}

void StudentWorld::spitVomit(Direction dir, Actor* zombie)
{
	switch (dir)
	{
	case GraphObject::left:

		if (oneInThree()) {
			addActor(new Vomit(this, zombie->getX() - SPRITE_WIDTH, zombie->getY(), 180));
			playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
		break;
	case GraphObject::right:
		if (oneInThree()) {
			addActor(new Vomit(this, zombie->getX() + SPRITE_WIDTH, zombie->getY(), 0));
			playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
		break;
	case GraphObject::down:
		if (oneInThree()) {
			addActor(new Vomit(this, zombie->getX(), zombie->getY() - SPRITE_HEIGHT, 270));
			playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
		break;
	case GraphObject::up:
		if (oneInThree()) {
			addActor(new Vomit(this, zombie->getX(), zombie->getY() + SPRITE_HEIGHT, 90));
			playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
		break;
	}
}

bool StudentWorld::isBlocked(Direction x, Actor* currentActor, int pixel)
{
	for (int i = 0; i < obj123.size(); i++)
	{
		if (obj123[i] == currentActor || !obj123[i]->blocksMovement())
			continue;										//basically, if the move would put the actor within the other's box, return true
		if (x == 0 &&
			currentActor->getX() + SPRITE_WIDTH - 1 + pixel >= obj123[i]->getX() &&
			currentActor->getX() + SPRITE_WIDTH - 1 + pixel <= obj123[i]->getX() + SPRITE_WIDTH - 1 &&
			currentActor->getY() + SPRITE_HEIGHT - 1 >= obj123[i]->getY() &&
			currentActor->getY() <= obj123[i]->getY() + SPRITE_HEIGHT - 1)
			return true;
		else if (x == 180 &&
			currentActor->getX() - pixel <= obj123[i]->getX() + SPRITE_WIDTH - 1 &&
			currentActor->getX() - pixel >= obj123[i]->getX() &&
			currentActor->getY() + SPRITE_HEIGHT - 1 >= obj123[i]->getY() &&
			currentActor->getY() <= obj123[i]->getY() + SPRITE_HEIGHT - 1)
			return true;
		else if (x == 90 &&
			currentActor->getY() + SPRITE_HEIGHT - 1 + pixel >= obj123[i]->getY() &&
			currentActor->getY() + SPRITE_HEIGHT - 1 + pixel <= obj123[i]->getY() + SPRITE_HEIGHT - 1 &&
			currentActor->getX() + SPRITE_WIDTH - 1 >= obj123[i]->getX() &&
			currentActor->getX() <= obj123[i]->getX() + SPRITE_WIDTH - 1)
			return true;
		else if (x == 270 &&
			currentActor->getY() - pixel <= obj123[i]->getY() + SPRITE_HEIGHT - 1 &&
			currentActor->getY() - pixel >= obj123[i]->getY() &&
			currentActor->getX() + SPRITE_WIDTH - 1 >= obj123[i]->getX() &&
			currentActor->getX() <= obj123[i]->getX() + SPRITE_WIDTH - 1)
			return true;
	}
	return false;
}

double StudentWorld::euclideanDis(Actor* x, Actor* y)
{
	return sqrt(pow(x->getX() - y->getX(), 2) + pow(x->getY() - y->getY(), 2) * 1.0);
}

double StudentWorld::euclideanDisOfPts(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) * 1.0);
}

double StudentWorld::distanceFromAnActorToPen(Actor* current)
{
	return euclideanDis(current, p);
}

double StudentWorld::distanceFromACitizenToPen(Actor* citizen, Direction& dir)
{
	if (citizen->getX() < getPlayer()->getX() && citizen->getY() < getPlayer()->getY())
		dir = citizen->randomFromTwoNumbers(0, 90);
	else if (citizen->getX() < getPlayer()->getX() && citizen->getY() > getPlayer()->getY())
		dir = citizen->randomFromTwoNumbers(0, 270);
	else if (citizen->getX() > getPlayer()->getX() && citizen->getY() < getPlayer()->getY())
		dir = citizen->randomFromTwoNumbers(180, 90);
	else if (citizen->getX() > getPlayer()->getX() && citizen->getY() > getPlayer()->getY())
		dir = citizen->randomFromTwoNumbers(180, 270);
	else if (citizen->getX() < getPlayer()->getX() && citizen->getY() == getPlayer()->getY())
		dir = 0;
	//if this citizen is to the left of Penelope and same row
	else if (citizen->getX() > getPlayer()->getX() && citizen->getY() == getPlayer()->getY())
		dir = 180;
	//if this citizen is to the right of Penelope and same row
	else if (citizen->getX() == getPlayer()->getX() && citizen->getY() < getPlayer()->getY())
		dir = 90;
	//if this citizen is below Penelope and same col
	else if (citizen->getX() == getPlayer()->getX() && citizen->getY() > getPlayer()->getY())
		dir = 270;
	//if this citizen is above Penelope and same col
	return euclideanDis(citizen, p);
}

double StudentWorld::distanceFromACitizenToAZombie(Actor* citizen, Direction& dir)
{
	double min = 1000;
	Actor* holder = nullptr;
	for (int i = 0; i < obj123.size(); i++)
	{
		if (obj123[i]->canVomit() && min > euclideanDis(citizen, obj123[i])) {
			min = euclideanDis(citizen, obj123[i]);
			holder = obj123[i];
		}
	}

	if (holder != nullptr) {
		if (citizen->getX() < holder->getX() && citizen->getY() < holder->getY())
			dir = citizen->randomFromTwoNumbers(180, 270);
		else if (citizen->getX() < holder->getX() && citizen->getY() > holder->getY())
			dir = citizen->randomFromTwoNumbers(180, 90);
		else if (citizen->getX() > holder->getX() && citizen->getY() < holder->getY())
			dir = citizen->randomFromTwoNumbers(0, 270);
		else if (citizen->getX() > holder->getX() && citizen->getY() > holder->getY())
			dir = citizen->randomFromTwoNumbers(0, 90);
		else if (citizen->getX() < holder->getX() && citizen->getY() == holder->getY())
			dir = 180;
		//if this citizen is to the left of Penelope and same row
		else if (citizen->getX() > holder->getX() && citizen->getY() == holder->getY())
			dir = 0;
		//if this citizen is to the right of Penelope and same row
		else if (citizen->getX() == holder->getX() && citizen->getY() < holder->getY())
			dir = 270;
		//if this citizen is below Penelope and same col
		else if (citizen->getX() == holder->getX() && citizen->getY() > holder->getY())
			dir = 90;
		//if this citizen is above Penelope and same col
	}
	return min;
}

Actor* StudentWorld::getClosestVictim(Actor* citizen, Direction& dir)
{	
	double min = 1000;
	Actor* holder;
	for (int i = 0; i < obj123.size(); i++)
	{
		if (obj123[i]->canGetInfected() && min > euclideanDis(citizen, obj123[i])) {
			min = euclideanDis(citizen, obj123[i]);
			holder = obj123[i];
		}
	}

	if (holder != nullptr) {

		if (citizen->getX() < holder->getX() && citizen->getY() < holder->getY())
			dir = citizen->randomFromTwoNumbers(0, 90);
		else if (citizen->getX() < holder->getX() && citizen->getY() > holder->getY())
			dir = citizen->randomFromTwoNumbers(0, 270);
		else if (citizen->getX() > holder->getX() && citizen->getY() < holder->getY())
			dir = citizen->randomFromTwoNumbers(180, 90);
		else if (citizen->getX() > holder->getX() && citizen->getY() > holder->getY())
			dir = citizen->randomFromTwoNumbers(180, 270);
		else if (citizen->getX() < holder->getX() && citizen->getY() == holder->getY())
			dir = 0;
		//if this citizen is to the left of Penelope and same row
		else if (citizen->getX() > holder->getX() && citizen->getY() == holder->getY())
			dir = 180;
		//if this citizen is to the right of Penelope and same row
		else if (citizen->getX() == holder->getX() && citizen->getY() < holder->getY())
			dir = 90;
		//if this citizen is below Penelope and same col
		else if (citizen->getX() == holder->getX() && citizen->getY() > holder->getY())
			dir = 270;
		//if this citizen is above Penelope and same col
	}
	
	return holder;
	//returns distance to the closest human
}

double StudentWorld::calcDestX(int dir, double currentX, int pixel)
{
	if (dir == 0)
		return currentX + pixel;
	else
		return currentX - pixel;
}
double StudentWorld::calcDestY(int dir, double currentY, int pixel)
{
	if (dir == 90)
		return currentY + pixel;
	else
		return currentY - pixel;
}

double StudentWorld::returnMax(double a, double b)			//RETURNS THE MAXIMUM
{
	if (a > b)
		return a;
	else
		return b;
}

void StudentWorld::vomitInfectingAllNearbyObjects(Actor* vomitActor)
{
	for (int i = 0; i < obj123.size(); i++) {
		if (!obj123[i]->canGetInfected())
			continue;
		if (euclideanDis(vomitActor, obj123[i]) <= 10) {
			if (obj123[i] != p && !obj123[i]->isInfectedOrNot())
				playSound(SOUND_CITIZEN_INFECTED);
			obj123[i]->setInfected();
		}
	}
}

void StudentWorld::damageAllNearbyObjects(Actor* flameActor)
{
	for (int i = 0; i < obj123.size(); i++) {
		if (euclideanDis(flameActor, obj123[i]) <= 10 && obj123[i]->isDamagedByFlame()) {
			if (obj123[i]->canGetInfected() && obj123[i] == p)
				playSound(SOUND_PLAYER_DIE);
			else if (obj123[i]->canGetInfected()) {
				playSound(SOUND_CITIZEN_DIE);
				increaseScore(-1000);
			}
			else if(obj123[i]->canFollowP() && obj123[i]->canVomit()){
				playSound(SOUND_ZOMBIE_DIE);
				increaseScore(2000);
			}
			else if (obj123[i]->canVomit()) {
				playSound(SOUND_ZOMBIE_DIE);
				increaseScore(1000);
			}
			obj123[i]->setDead();
		}
	}
}

void StudentWorld::swallowAllNearbyObjects(Actor* pitActor)
{
	for (int i = 0; i < obj123.size(); i++) {
		if (obj123[i]->blocksMovement() && euclideanDis(pitActor, obj123[i]) <= 10) {
			if (obj123[i] == p)
				playSound(SOUND_PLAYER_DIE);
			else if (obj123[i]->canGetInfected())
				playSound(SOUND_CITIZEN_DIE);
			else if(obj123[i]->canVomit())
				playSound(SOUND_ZOMBIE_DIE);
			obj123[i]->setDead();
			obj123[i]->setDead();
		}
	}
}

void StudentWorld::explodeAllNearbyObjects(Actor* bombActor)
{
	for (int i = 0; i < obj123.size(); i++) {
		if (obj123[i]->triggersOnlyActiveLandmines() && euclideanDis(bombActor, obj123[i]) <= 10) {
			bombActor->setDead();
			playSound(SOUND_LANDMINE_EXPLODE);
			addActor(new Flame(this, bombActor->getX(), bombActor->getY(), GraphObject::up));
			addActor(new Flame(this, bombActor->getX(), bombActor->getY() + SPRITE_HEIGHT, GraphObject::up));
			addActor(new Flame(this, bombActor->getX(), bombActor->getY() - SPRITE_HEIGHT, GraphObject::up));
			addActor(new Flame(this, bombActor->getX() + SPRITE_WIDTH, bombActor->getY(), GraphObject::up));
			addActor(new Flame(this, bombActor->getX() + SPRITE_WIDTH, bombActor->getY() + SPRITE_HEIGHT, GraphObject::up));
			addActor(new Flame(this, bombActor->getX() + SPRITE_WIDTH, bombActor->getY() - SPRITE_HEIGHT, GraphObject::up));
			addActor(new Flame(this, bombActor->getX() - SPRITE_WIDTH, bombActor->getY(), GraphObject::up));
			addActor(new Flame(this, bombActor->getX() - SPRITE_WIDTH, bombActor->getY() + SPRITE_HEIGHT, GraphObject::up));
			addActor(new Flame(this, bombActor->getX() - SPRITE_WIDTH, bombActor->getY() - SPRITE_HEIGHT, GraphObject::up));
			addActor(new Pit(this, bombActor->getX(), bombActor->getY()));
			return;
		}
	}
}

bool StudentWorld::isZombieVomitTriggerAt(double x, double y)
{
	for (int i = 0; i < obj123.size(); i++)
	{
		if (obj123[i]->canGetInfected() && euclideanDisOfPts(x, y, obj123[i]->getX(), obj123[i]->getY()) <= 10)
			return true;
	}
	return false;
}

bool StudentWorld::isFlameBlocked(double x, double y)
{
	for (int i = 0; i < obj123.size(); i++)
	{
		if (!obj123[i]->blocksFlame())
			continue;										//basically, if the move would put the actor within the other's box, return true
		if (euclideanDisOfPts(x, y, obj123[i]->getX(), obj123[i]->getY()) <= 10)
			return true;
	}
	return false;
}

bool StudentWorld::overlapsWithCitizen(Actor* exit)
{
	for (int i = 0; i < obj123.size(); i++)
	{
		if (obj123[i] != p &&
			obj123[i]->canGetInfected() &&
			euclideanDisOfPts(exit->getX(), exit->getY(), obj123[i]->getX(), obj123[i]->getY()) <= 10)
		{
			obj123[i]->setDead();
			return true;
		}
	}
	return false;
}

bool StudentWorld::oneInThree()
{
	bool TrueFalse = (rand() % 100) < 33;
	return TrueFalse;
}

bool StudentWorld::oneInSeven()
{
	bool TrueFalse = (rand() % 100) < 70;
	return TrueFalse;
}


void StudentWorld::addActor(Actor* a)
{
	obj123.push_back(a);
}

void StudentWorld::cleanUp()
{
	for (int i = 0; i < obj123.size(); i++)
		delete obj123[i];
	obj123.clear();
	p = nullptr;
}


