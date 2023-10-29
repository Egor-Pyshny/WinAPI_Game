#pragma once
#include <math.h>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class target
{
public:
	int section;
	int x;
	int y;
	int ttl;
	int width;
	int height;
	target(int section, int x, int y, int ttl);
	void setImage(Bitmap* bmp);
	Bitmap* getImage();
	int getPoints(Point bullet_point);
	static const int types = 3;
private:
	static const int max_sections = 5;
	Bitmap* image;
	Point getCenter();
	double distance(Point c, Point bullet_point);
	int awards[types][max_sections] = {
		{3, 2, 1, 0, 0},
		{4, 3, 2, 1, 0},
		{5, 4, 3, 2, 1},
	};
};
