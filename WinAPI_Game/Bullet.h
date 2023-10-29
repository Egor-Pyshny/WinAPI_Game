#pragma once
class Bullet
{
public:
	int x;
	int y;
	Bullet(int x, int y);
	Bullet();
	float getRadius();
private:
	float radius = 2.5;
};

