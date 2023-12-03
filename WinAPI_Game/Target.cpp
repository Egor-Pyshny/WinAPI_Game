#include "Target.h"

target::target(int section, int x, int y, int ttl)
{
	this->section = section;
	this->x = x;
	this->y = y;
	this->ttl = ttl;
	this->image = NULL;
	this->width = 25 + (17 * (section - 1)) * 2;
	this->height = this->width;
}

target::target(int section, int ttl)
{
	this->section = section;
	this->ttl = ttl;	
	this->width = 25 + (17 * (section - 1)) * 2;
	this->height = this->width;
}

void target::setImage(Bitmap* bmp)
{
	this->image = bmp;
}

void target::setX(int x)
{
	this->x = x;
}

void target::setY(int y)
{
	this->y = y;
}

Bitmap* target::getImage()
{
	return this->image;
}

int target::getX()
{
	return this->x;
}

int target::getY()
{
	return this->y;
}

int target::getWidth()
{
	return this->width;
}

int target::getHeight()
{
	return this->height;
}

int target::getTTL()
{
	return this->ttl;
}

int target::getPoints(Point bullet_point)
{
	Point c = this->getCenter();
	double d = this->distance(c, bullet_point) - 12;
	int temp = 0;
	int delta = 17;
	for (int i = 0; i < max_sections; i++) {
		if (d < temp) {
			return awards[this->section - 3][i];
		}
		temp += 17;
	}
	return 0;
}

Point target::getCenter()
{
	int delta = (this->section - 1) * 17 + 12;
	return Point(this->x + delta, this->y + delta);
}

double target::distance(Point c, Point bullet_point)
{
	double tmp1 = pow(((double)c.X - bullet_point.X - 12), 2), tmp2 = pow(((double)c.Y - bullet_point.Y - 12), 2);
	double distance = sqrt(tmp1 + tmp2);
	return distance;
}
