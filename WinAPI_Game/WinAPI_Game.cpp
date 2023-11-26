#ifndef UNICODE
#define UNICODE
#endif 

#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
//#include <iostream>
//#include <stdio.h>
//#include <wchar.h>
//#include <commctrl.h>
#include <Windows.h>
//#include <winuser.h>
#include <string>
#include <gdiplus.h>
#include <vector>
#include "Resource.h"
#include "Scope.h"
#include "Target.h"
#include <queue>
#include "Bullet.h"
#include <windowsx.h>

using namespace Gdiplus;
using namespace std;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializePhotos();
DWORD WINAPI InitilizeTargets(LPVOID lpParam);
DWORD WINAPI GetData(LPVOID lpParam);
DWORD WINAPI RefreshPosition(LPVOID lpParam);

Bitmap* imageList[target::types];
const int max_bullets_to_draw = 50;
Bullet bullets[max_bullets_to_draw];
Scope scope;
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;
HBITMAP hOldBitmap = NULL;
Bitmap* scope_png = NULL;
HANDLE g_mutex;
vector<target> targets;
target* current_target = NULL;
queue<Point> points;
const UINT_PTR fps_timer_id = 1;
const UINT_PTR draw_timer_id = 2;
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;
const int FPS_LIMIT = 50;
int scope_x;
int scope_y;
int score = 0;
int current_target_number = -1;
int bullet_number = 0;
bool isplaying = true;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    InitializePhotos();
    HANDLE hThread = CreateThread(nullptr, 0, InitilizeTargets, NULL, 0, nullptr);
    if (hThread == NULL) {
        CloseHandle(g_mutex);
        return 1;
    }
    g_mutex = CreateMutexW(NULL, FALSE, NULL);
    if (g_mutex == NULL) {
        CloseHandle(hThread);
        return 1;
    }
    const wchar_t CLASS_NAME[] = L"Game";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        0,                                
        CLASS_NAME,                       
        L"Time killer",                   
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH+16, WINDOW_HEIGHT+39,
        NULL,       
        NULL,       
        hInstance,  
        NULL        
    );
    if (hwnd == NULL)
    {
        return 0;
    }
    WaitForSingleObject(g_mutex, INFINITE);
    SetTimer(hwnd, fps_timer_id, 1000 / FPS_LIMIT, NULL);
    SetTimer(hwnd, draw_timer_id, 0, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    HANDLE data_thread = CreateThread(nullptr, 0, GetData, NULL, 0, nullptr);
    if (data_thread == NULL) {
        return 1;
    }
    HANDLE refresh_thread = CreateThread(nullptr, 0, RefreshPosition, NULL, 0, nullptr);
    if (refresh_thread == NULL) {
        return 1;
    }
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    CloseHandle(hThread);
    CloseHandle(data_thread);
    CloseHandle(refresh_thread);
    CloseHandle(g_mutex);
    return (int)msg.wParam;
}

DWORD WINAPI InitilizeTargets(LPVOID lpParam) {
    target t1(3, 150, 150, 2);
    t1.setImage(imageList[3 - 3]);
    targets.push_back(t1);
    ReleaseMutex(g_mutex);
    return 0;
}

DWORD WINAPI GetData(LPVOID lpParam) {
    int port = 8888;
    string ipAddressStr = "127.0.0.1";
    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &data) != 0) {
        throw 1;
    }
    sockaddr_in localEndPoint;
    localEndPoint.sin_family = AF_INET;
    localEndPoint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddressStr.c_str(), &localEndPoint.sin_addr);
    SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        WSACleanup();
        throw 1;
    }
    if (connect(listener, (sockaddr*)&localEndPoint, sizeof(localEndPoint)) == SOCKET_ERROR) {
        closesocket(listener);
        WSACleanup();
        throw 1;
    }
    char buffer[4];
    int i = 1;
    float f_y;
    float f_x;
    while (true) {
        while (isplaying) {
            int g = recv(listener, buffer, 4, 0);
            if (g != 0) {
                if (i == 2) f_y = *((float*)(buffer)); else if (i == 1) f_x = *((float*)(buffer));
                if (i != 3) {
                    i++;
                }
                else if (i == 3) {
                    points.push(Point(f_x, f_y));
                    i == 1;
                }
            }
        }
    }
    closesocket(listener);
    WSACleanup();
    return 0;
}

DWORD WINAPI RefreshPosition(LPVOID lpParam) {
    while (isplaying) {
        if (points.size() > 0) {
            Point p = points.front();
            points.pop();
            scope.move_by_angles(p.X,p.Y);
        }
    }
    return 0;
}

void RestartGame(HWND hwnd) {
    isplaying = true;
    scope.reset();
    scope_x = scope.getX();
    scope_y = scope.getY();
    score = 0;
    current_target_number = -1;
    bullet_number = 0;
    for (int i = 0; i < max_bullets_to_draw; i++) bullets[i] = Bullet();
    SetTimer(hwnd, draw_timer_id, 0, NULL);
}

void Shoot() {
    if (bullet_number <= max_bullets_to_draw - 1) {
        bullets[bullet_number] = Bullet(scope.getX() + 10, scope.getY() + 10);
        bullet_number++;
    }
    else {
        bullets[0] = Bullet(scope.getX() + 10, scope.getY() + 10);
        bullet_number = 1;
    }
}

void InitializePhotos() {
    scope_png = Bitmap::FromFile(L"scope.png", FALSE);
    WCHAR name[100];
    for (int i = 0; i < target::types; i++) {
        swprintf(name, sizeof(name) / sizeof(wchar_t), L"%dsection_target.png", i+3);
        imageList[i] = Bitmap::FromFile(name, FALSE);
    }
}

void DrawDivider(HDC hdc) {
    RECT rect = { 600, 0, 602, 600 };
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
}

void DrawScope(HDC hdc) {
    Graphics graphics(hdc); 
    UINT width = scope_png->GetWidth();
    UINT height = scope_png->GetHeight();
    graphics.DrawImage(scope_png, scope_x, scope_y, width, height);
}

void DrawTarget(HDC hdc) {
    Graphics graphics(hdc);
    graphics.DrawImage(current_target->getImage(), current_target->x, current_target->y, current_target->width, current_target->height);
}

void DrawBullets(HDC hdc) {
    HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hRedBrush);
    for (int i = 0; i < max_bullets_to_draw; i++) {
        if (bullets[i].x != -1) {
            Ellipse(hdc, bullets[i].x, bullets[i].y, bullets[i].x + 5, bullets[i].y + 5);
        }
    }
    SelectObject(hdc, hOldBrush);
    DeleteObject(hRedBrush);
}

void DrawScoreText(HDC hdc) {
    RECT text_rect = { 600,20,900,70 };
    string temp = "Score: ";
    temp += to_string(score);
    temp += "\0\0";
    wstring text(temp.begin(), temp.end());
    DrawTextW(hdc, text.c_str(), -1, &text_rect, DT_CENTER);
}

void DrawRestartButton(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    RECT restart_game = { 622,550,880,580 };
    RECT restart_game_text = { 620,555,880,580 };
    HBRUSH hBrush = CreateSolidBrush(RGB(181, 181, 181));
    FillRect(hdc, &restart_game, hBrush);
    DeleteObject(hBrush);
    const WCHAR text[] = L"Restart";
    int textSize = ARRAYSIZE(text);
    DrawTextW(hdc, text, textSize, &restart_game_text, DT_CENTER | DT_VCENTER);
}

void SwitchTarget() {
    int a = targets.size();
    if ((current_target_number < a-1) && isplaying) {  
        current_target_number++;
        current_target = &targets[current_target_number];
    }
    else {
        current_target = NULL;
        current_target_number = -1;
        isplaying = false;        
    }
}

void CalculateScore() {
    score += current_target->getPoints(Point(scope.getX(), scope.getY()));
}

void FillBackground(HWND hwnd, HDC hdc) {
    HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    FillRect(hdc, &clientRect, hWhiteBrush);
    DeleteObject(hWhiteBrush);
}

void DoubleBuff(HWND hwnd) {
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    HDC hdc = GetDC(hwnd);
    hdcBuffer = CreateCompatibleDC(hdc);
    hBitmap = CreateCompatibleBitmap(hdc, width, height);
    hOldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmap);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        KillTimer(hwnd, fps_timer_id);
        return 0;

    case WM_CREATE:
    {
        scope.reset();
        scope_x = scope.getX();
        scope_y = scope.getY();
        break;
    }
    case WM_MOUSEMOVE: 
    {
        break;
    }
    case WM_KEYDOWN:
    {
        scope.move_by_keys(wParam);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillBackground(hwnd, hdcBuffer);
        DrawDivider(hdcBuffer);
        DrawScoreText(hdcBuffer);
        if (isplaying) {
            if (current_target != NULL) {
                DrawTarget(hdcBuffer);
            }
            DrawBullets(hdcBuffer);
        }
        else {
            DrawRestartButton(hdcBuffer);
        }
        DrawScope(hdcBuffer);
        BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcBuffer, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONUP: 
    {
        if (!isplaying) {
            RECT restart_game = { 620,550,720,580 };
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if ((xPos >= 620 && xPos <= 720) &&
                (yPos >= 550 && yPos <= 580)) {
                RestartGame(hwnd);
            }
        }
        break;
    }
    case WM_TIMER: 
    {
        switch ((UINT_PTR)wParam)
        {
        case fps_timer_id: 
        {
            scope_x = scope.getX();
            scope_y = scope.getY();
            if (isplaying) {
                Shoot();
                CalculateScore();
            }
            DoubleBuff(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);

            break;
        }
        case draw_timer_id:
        {
            KillTimer(hwnd, draw_timer_id);
            SwitchTarget();
            if(isplaying)
                SetTimer(hwnd, draw_timer_id, 1000 * current_target->ttl, NULL);
        }
        default:
            break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}