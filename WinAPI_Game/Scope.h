#pragma once

#include <windows.h>
#include <cmath>
#include <queue>
#include "Logger.h"

class Scope
{
public:
	bool move_by_keys(WPARAM wparam);
	void move_by_angles(float x, float y);
	void move_by_angles(POINTFLOAT p);
	void reset();
	int getX();
	int getY();
	void setLogger(Logger* log);
	void setX(int x);
	void setY(int y);
	void setWorkingAreaWidth(int working_area_width);
	void setWorkingAreaHeight(int working_area_height);
	void setMaxXAngle(float maxXAngle);
	void setMaxYAngle(float maxYAngle);
	void setMinXAngle(float minXAngle);
	void setMinYAngle(float minYAngle);
	void setCenterXAngle(float centerXAngle);
	void setCenterYAngle(float centerYAngle);
private:
	std::queue<POINT> points;
	Logger* log;
	const int step = 10;
	const int max = 17;
	float fault = 0.07;
	float maxXAngle = 0;
	float maxYAngle = 0;
	float minXAngle = 0;
	float minYAngle = 0;
	float centerXAngle = 0;
	float centerYAngle = 0;
	int x = 0;
	int y = 0;
	int working_area_width;
	int working_area_height;
	void move_up();
	void move_down();
	void move_right();
	void move_left();
};

