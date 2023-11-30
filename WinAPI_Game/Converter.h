#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>
#include <windows.h>

class Converter
{
public:
	Converter(int width, int height, float maxAngleX, float maxAngleY);
	int ToCoordX(float angle);
	int ToCoordY(float angle);
	POINT ToCoord(POINTFLOAT point);
protected:
	int maxCoordX;
	int maxCoordY;

	float maxAngleX;
	float maxAngleY;
	float angleWidth;
	float angleHeight;

	int maxLogCoordX;
	int maxLogCoordY;
};