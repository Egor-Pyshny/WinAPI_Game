#pragma once
#include <math.h>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class target
{
public:
	target(int section, int x, int y, int ttl);
	target(int section, int ttl);
	void setImage(Bitmap* bmp);
	void setX(int x);
	void setY(int y);
	Bitmap* getImage();
	int getX();
	int getY();
	int getWidth();
	int getHeight();
	int getTTL();
	int getSections();
	int getPoints(Point bullet_point);
	static const int types = 3;
private:
	int section;
	int x;
	int y;
	int ttl;
	int width;
	int height;
	static const int max_sections = 5;
	Bitmap* image = NULL;
	Point getCenter();
	double distance(Point c, Point bullet_point);
	int awards[types][max_sections] = {
		{3, 2, 1, 0, 0},
		{4, 3, 2, 1, 0},
		{5, 4, 3, 2, 1},
	};
};
