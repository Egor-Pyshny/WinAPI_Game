#include "Bullet.h"

Bullet::Bullet(int x, int y)
{
	this->x = x;
	this->y = y;
}

Bullet::Bullet() { 
	this->x = -1;
	this->y = -1;
}

float Bullet::getRadius()
{
	return this->radius;
}
