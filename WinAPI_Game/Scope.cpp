#include "Scope.h"

void Scope::reset() {
    this->x = (this->working_area_width - 300) / 2 - 12;
    this->y = this->working_area_height / 2 - 12;
}

int Scope::getX()
{
    return this->x;
}

int Scope::getY()
{
    return this->y;
}

void Scope::setLogger(Logger* log)
{
    this->log = log;
    log->queues.coords_queue = &(this->points);
}

void Scope::setX(int x)
{
    this->x = x;
}

void Scope::setY(int y)
{
    this->y = y;
}

void Scope::setStep(int step)
{
    this->step = step;
}

void Scope::setWorkingAreaWidth(int working_area_width)
{
    this->working_area_width = working_area_width;
}

void Scope::setWorkingAreaHeight(int working_area_height)
{
    this->working_area_height = working_area_height;
}

void Scope::setXAngle(float minXAngle)
{
    this->XAngle = minXAngle;
}

void Scope::setYAngle(float minYAngle)
{
    this->YAngle = minYAngle;
}

void Scope::setCenterXAngle(float centerXAngle)
{
    this->centerXAngle = centerXAngle;
}

void Scope::setCenterYAngle(float centerYAngle)
{
    this->centerYAngle = centerYAngle;
}

void Scope::move_up()
{
    this->y -= step;
    if (this->y < 0) this->y = 0;
}

void Scope::move_down()
{
    this->y += step;
    if (this->y > this->working_area_height - 25) this->y = this->working_area_height - 25;
}

void Scope::move_right()
{
    this->x += step;
    if (this->x > this->working_area_width - 300 - 25) this->x = working_area_width - 300 - 25;
}

void Scope::move_left()
{
    this->x -= step;
    if (this->x < 0) this->x = 0;
}

void Scope::move_by_angles(POINTFLOAT p)
{
    float delta_x = 0-p.x;
    float delta_y = 0-p.y;
    if (abs(delta_x) > fault && abs(delta_x) > fault) {
        float scaling_coef_x = p.x / this->XAngle;
        float scaling_coef_y = p.y / this->YAngle;
        this->x += step * scaling_coef_x;
        this->y += step * scaling_coef_y;
        if (this->y < 0) this->y = 0;
        if (this->y > 575) this->y = 575;
        if (this->x > 575) this->x = 575;
        if (this->x < 0) this->x = 0;
    }
}

bool Scope::move_by_keys(WPARAM wparam) {
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
    POINT p(this->x, this->y);
    this->points.push(p);
    return true;
}
