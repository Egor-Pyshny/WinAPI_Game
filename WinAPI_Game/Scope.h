#pragma once

#include <windows.h>
#include <cmath>

class Scope
{
public:
	bool move_by_keys(WPARAM wparam);
	void move_by_angles(float x, float y);
	void move_by_angles(POINTFLOAT p);
	void reset();
	int getX();
	int getY();
	void setMaxXAngle(float maxXAngle);
	void setMaxYAngle(float maxYAngle);
	void setMinXAngle(float minXAngle);
	void setMinYAngle(float minYAngle);
	void setCenterXAngle(float centerXAngle);
	void setCenterYAngle(float centerYAngle);
	int x = 0;
	int y = 0;
private:
	const int step = 10;
	const int max = 17;
	float fault = 0.07;
	float maxXAngle = 0;
	float maxYAngle = 0;
	float minXAngle = 0;
	float minYAngle = 0;
	float centerXAngle = 0;
	float centerYAngle = 0;

	void move_up();
	void move_down();
	void move_right();
	void move_left();
};

