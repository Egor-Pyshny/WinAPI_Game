#include "Scope.h"

void Scope::reset() {
	this->x = 300-12;
	this->y = 300-12;
}

int Scope::getX()
{
    return this->x;
}

int Scope::getY()
{
    return this->y;
}

void Scope::move_up()
{
    this->y -= step;
    if (this->y < 0) this->y = 0;
}

void Scope::move_down()
{
    this->y += step;
    if (this->y > 575) this->y = 575;
}

void Scope::move_right()
{
    this->x += step;
    if (this->x > 575) this->x = 575;
}

void Scope::move_left()
{
    this->x -= step;
    if (this->x < 0) this->x = 0;
}

bool Scope::move(WPARAM wparam) {
    switch (wparam) {
    case VK_LEFT:
    {
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            move_left();
            move_up();
            break;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) 
        {
            move_left();
            move_down();
            break;
        }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) 
        {
            break;
        }
        move_left();
        break;
    }
    case VK_RIGHT:
    {
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            move_right();
            move_up();
            break;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            move_right();
            move_down();
            break;
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            break;
        }
        move_right();
        break;
    }
    case VK_UP:
    {
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            move_right();
            move_up();
            break;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            break;
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            move_up();
            move_left();
            break;
        }
        move_up();
        break;
    }
    case VK_DOWN:
    {
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            move_right();
            move_down();
            break;
        }
        else if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            break;
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            move_down();
            move_left();
            break;
        }
        move_down();
        break;
    }
    default:
        return false;
    }
    return true;
}