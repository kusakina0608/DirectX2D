#pragma once

#include <math.h>

// Base Animation Class

class Animation
{
public:
	Animation(float start, float end, float duration) :
		m_Start(start),
		m_End(end),
		m_Duration(duration)
	{
	}

	void SetStart(float start)
	{
		m_Start = start;
	}

	float GetStart()
	{
		return m_Start;
	}

	void SetEnd(float end)
	{
		m_End = end;
	}

	float GetEnd()
	{
		return m_End;
	}

	void SetDuration(float duration)
	{
		m_Duration = max(0, duration);
	}

	float GetDuration()
	{
		return m_Duration;
	}

	float GetValue(float time)
	{
		time = min(max(time, 0), m_Duration);
		return ComputeValue(time);
	}

protected:
	virtual float ComputeValue(float time) = 0;

	float m_Duration;
	float m_Start;
	float m_End;
};

class AnimationEaseInOut : public Animation
{
public:
	AnimationEaseInOut(float start = 0, float end = 0, float duration = 0) :
		Animation(start, end, duration)
	{
	}
protected:
	float ComputeValue(float time)
	{
		//compute the current time relative to the midpoint
		return (float)(m_Start + ((m_End - m_Start) * (time / m_Duration)));
		time = time / (m_Duration / 2);
		//if we haven't reached the midpoint, we want to do the ease-in portion
		if (time < 1)
		{
			return (float)(m_Start + (m_End - m_Start) / 2 * (-pow(2, -10 * --time) + 2));
		}
		//otherwise, do the ease-out portion
		return (float)(m_Start + (m_End - m_Start) / 2 * pow(2, 10 * (time - 1)));
	}
};
