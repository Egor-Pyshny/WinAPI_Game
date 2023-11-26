#pragma once
#include <windows.h>
class Scope
{
public:
	bool move_by_keys(WPARAM wparam);
	void move_by_angles(float x, float y);
	void reset();
	int getX();
	int getY();
private:
	int x = 0;
	int y = 0;
	const int step = 10;
	const int max = 17;
	void move_up();
	void move_down();
	void move_right();
	void move_left();
};

