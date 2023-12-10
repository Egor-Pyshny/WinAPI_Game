#include "Converter.h"

Converter::Converter(int width, int height, float maxAngleX, float maxAngleY)
{
	width = width;
	height = height;

	this->maxAngleX = maxAngleX;
	this->maxAngleY = maxAngleY;

	angleWidth = maxAngleX * 2;
	angleHeight = maxAngleY * 2;

	maxLogCoordX = width / 2;
	maxLogCoordY = height / 2;
}

int Converter::ToCoordX(float angle)
{
	angle = -angle + maxAngleX;
	return (int)round(angle * width / angleWidth);
}

int Converter::ToCoordY(float angle)
{
	angle = -angle + maxAngleY;
	return (int)round(angle * height / angleHeight);
}

POINT Converter::ToCoord(POINTFLOAT point)
{
	return { ToCoordX(point.x), ToCoordY(point.y) };
}