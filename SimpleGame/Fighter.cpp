#include "Fighter.h"


Fighter::Fighter(Type _type, DWORD stateTransitions[][3], int numTransitions)//, MeshMD2* _model)
{
	//model = _model; //copy
	type = _type;

	// 유한상태기계를 만든다.
	stateMachine = new FiniteStateMachine();
	for (int i = 0; i < numTransitions; i++)
	{
		stateMachine->addStateTransition(stateTransitions[i][0], stateTransitions[i][1], stateTransitions[i][2]);
	}
	stateMachine->setCurrentStateID(STATE_STOP);

	// 캐릭터 특성을 결정한다.
	// 초기화한다.
	position = D2D1::Point2F(300, 0);
	setDestPosition(position);
	Frame = 0;
	ChatTrigger = 0;
}

Fighter::~Fighter()
{
	if (stateMachine != 0)
		delete stateMachine;
}

void Fighter::update()
{
	switch (stateMachine->getCurrentStateID())
	{
	case STATE_STOP:
		if (counter++ > 240)
		{
			if ((rand() % 3 == 0))
			{
				counter = 0;
				issueEvent(Fighter::EVENT_STARTWALK);
				Faced = rand() % 2 ? RIGHT : LEFT;
			}
		}
		break;
	case STATE_WALK:
		if (Faced) {
			position.x += 1;
		}
		else {
			position.x -= 1;
		}
		if (Faced==LEFT&&position.x < -200)
		{
			Faced = RIGHT;
		}
		else if (Faced == RIGHT && position.x > 500)
		{
			Faced = LEFT;
		}
		if (counter++ > 240)
		{
			if ((rand() % 3 == 0))
			{
				counter = 0;
				issueEvent(Fighter::EVENT_STOPWALK);
			}
		}
		break;
	case STATE_TALK:
		break;
	case STATE_MOVE:
		if (position.x >= -195)
		{
			if (200 + position.x > 0) {
				position.x -= 1;
				Faced = LEFT;
			}
			else {
				position.x += 1;
				Faced = RIGHT;
			}
		}
		else {
			Faced = RIGHT;
			issueEvent(Fighter::EVENT_REACHED);
		}
		break;
	case STATE_ATTACK:
		break;
	}
}

void Fighter::actionStand(float timeDelta) { }
void Fighter::actionMove(float timeDelta) { moveTo(timeDelta); }
void Fighter::actionFollow(float timeDelta) { moveTo(timeDelta); }
void Fighter::actionAttack(float timeDelta) { }
void Fighter::actionRunaway(float timeDelta) { }


void Fighter::issueEvent(DWORD event)
{
	stateMachine->issueEvent(event);

	switch (stateMachine->getCurrentStateID())
	{
	case STATE_STOP:
		break;
	case STATE_WALK:
		break;
	case STATE_TALK:
		break;
	case STATE_MOVE:
		break;
	case STATE_ATTACK:
		break;
	}

}

void Fighter::moveTo(float timeDelta)
{
	position.x += 1;

	if (GetLengthBetween(destPosition, position) < 1)
	{
		issueEvent(Fighter::EVENT_STOPWALK);
	}
}

void Fighter::setDestPosition(D2D1_POINT_2F dest)
{
	destPosition = dest;
}

double GetLengthBetween(D2D1_POINT_2F p1, D2D1_POINT_2F p2)
{
	return abs(p1.x - p2.x);
}