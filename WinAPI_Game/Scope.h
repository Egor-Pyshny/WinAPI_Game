#pragma once
#include <windows.h>
class Scope
{
public:
	bool move(WPARAM wparam);
	void reset();
	int getX();
	int getY();
private:
	int x = 0;
	int y = 0;
	const int step = 7;
	void move_up();
	void move_down();
	void move_right();
	void move_left();
};

