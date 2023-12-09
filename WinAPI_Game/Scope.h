#pragma once

#include <windows.h>
#include <cmath>
#include <queue>
#include "Logger.h"

class Scope
{
public:
	bool move_by_keys(WPARAM wparam);
	void move_by_angles(POINTFLOAT p);
	void reset();
	int getX();
	int getY();
	void setLogger(Logger* log);
	void setX(int x);
	void setY(int y);
	void setStep(int step);
	void setWorkingAreaWidth(int working_area_width);
	void setWorkingAreaHeight(int working_area_height);
	void setXAngle(float maxXAngle);
	void setYAngle(float maxYAngle);
	void setCenterXAngle(float centerXAngle);
	void setCenterYAngle(float centerYAngle);
private:
	std::queue<POINT> points;
	Logger* log;
	int step = 10;
	float fault = 0.07;
	float XAngle = 0;
	float YAngle = 0;
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

