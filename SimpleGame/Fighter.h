#pragma once
#include <d2d1.h>
#include "FiniteStateMachine.h"

#define LEFT 0
#define RIGHT 1

class Fighter
{
public:
	enum StateID {
		STATE_UNDEFINED = 0,
		STATE_STOP,
		STATE_WALK,
		STATE_TALK,
		STATE_MOVE,
		STATE_ATTACK
	};
	enum EventID {
		EVENT_UNDEFINED = 0,
		EVENT_STARTWALK,
		EVENT_STOPWALK,
		EVENT_TALK,
		EVENT_YES,
		EVENT_NO,
		EVENT_REACHED,
		EVENT_TERMINATE
	};
	enum Type {
		TYPE_UNDEFINED = 0,
		TYPE_AI,
	};
private:
	Type type;
	D2D1_POINT_2F position;
	D2D1_POINT_2F destPosition;
	FiniteStateMachine* stateMachine;
	unsigned int Frame = 0;
	unsigned int ChatTrigger = 0;
	unsigned int Faced = RIGHT;
	unsigned int counter = 0;


public:
	Fighter(Type _type, DWORD stateTransitions[][3], int numTransitions);//, MeshMD2* _model);
	~Fighter();

	void update();

	void setPosition(D2D1_POINT_2F pos) { position = pos; }
	D2D1_POINT_2F getPosition() { return position; }
	void setDestPosition(D2D1_POINT_2F dest);
	D2D1_POINT_2F getDestPositon() { return destPosition; }
	void setFrame(int frame) { Frame = frame; }
	int getFrame() { return Frame; }
	void setChatTrigger(int frame) { ChatTrigger = frame; }
	int getChatTrigger() { return ChatTrigger; }
	int getState() { return stateMachine->getCurrentStateID(); }
	int getFaced() { return Faced; }
	void issueEvent(DWORD event);

private:
	void moveTo(float timeDelta);

	void actionStand(float timeDelta);
	void actionMove(float timeDelta);
	void actionFollow(float timeDelta);
	void actionAttack(float timeDelta);
	void actionRunaway(float timeDelta);
};

double GetLengthBetween(D2D1_POINT_2F p1, D2D1_POINT_2F p2);