#ifndef UNICODE
#define UNICODE
#endif 
#define _USE_MATH_DEFINES

#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <Windows.h>
#include <string>
#include <gdiplus.h>
#include <vector>
#include "Resource.h"
#include "Scope.h"
#include "Target.h"
#include <queue>
#include "Bullet.h"
#include <windowsx.h>
#include <cmath>
#include "Connection.h"
#include "Converter.h"

using namespace Gdiplus;
using namespace std;

LRESULT CALLBACK Game_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Settings_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int DrawComponents(HWND hwnd, HINSTANCE hInstance);
void InitializePhotos();
DWORD WINAPI InitilizeTargets(LPVOID lpParam);
DWORD WINAPI GetData(LPVOID lpParam);

vector<int> v;

HWND hLabelX;
HWND hLabelY;
HWND hLabelSreen;
HWND hwndButtonStartCalibratingX;
HWND hwndButtonStopCalibratingX;
HWND hwndButtonStartCalibratingY;
HWND hwndButtonStopCalibratingY;
HWND hwndButtonStartCalibratingCenter;
HWND hwndButtonStopCalibratingCenter;
HWND hwndButtonStartGame;
HWND hComboBox;
HWND hwndXTexbox;
HWND hwndYTexbox;
HWND hwndGameWindow;
HWND hwndSettingsWindow;
HANDLE hTargetsThread;
HFONT hTextBoxFont;
bool EnableStopX = false;
bool EnableStopY = false;
bool EnableStopCenter = false;
bool XCalibrated = false;
bool YCalibrated = false;
bool CenterCalibrated = false;
Bitmap* imageList[target::types];
const int max_bullets_to_draw = 30;
Bullet bullets[max_bullets_to_draw];
Scope scope;
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;
HBITMAP hOldBitmap = NULL;
Bitmap* scope_png = NULL;
vector<target> targets;
target* current_target = NULL;
queue<Point> points;
POINTFLOAT currentAngles;
const wchar_t* resolutions[] = IDC_RESOLUTIONLIST;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
const int FPS_LIMIT = 50;
int score = 0;
int current_target_number = -1;
int bullet_number = 0;
bool isplaying = false;
float maxXAngle = 0;
float maxYAngle = 0;
float minXAngle = 0;
float minYAngle = 0;
float centerXAngle = 0;
float centerYAngle = 0;
int fps = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    InitializePhotos();
    hTargetsThread = CreateThread(nullptr, 0, InitilizeTargets, NULL, 0, nullptr);
    if (hTargetsThread == NULL) {
        return 1;
    }

    WNDCLASS wc_settings = { };
    wc_settings.lpfnWndProc = Settings_WindowProc;
    wc_settings.hInstance = hInstance;
    wc_settings.lpszClassName = L"Settings";
    wc_settings.style = CS_HREDRAW | CS_VREDRAW;
    wc_settings.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc_settings);
    hwndSettingsWindow = CreateWindowEx(
        0,
        L"Settings",
        L"Settings",
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwndSettingsWindow == NULL)
    {
        return 1;
    }
    RECT newClientRect = { 0, 0, 400, 400 };
    AdjustWindowRect(&newClientRect, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(
        hwndSettingsWindow,
        NULL,
        0, 0,
        newClientRect.right - newClientRect.left,
        newClientRect.bottom - newClientRect.top,
        SWP_NOMOVE | SWP_NOZORDER
    );

    
    WNDCLASS wc_game = { };
    wc_game.lpfnWndProc = Game_WindowProc;
    wc_game.hInstance = hInstance;
    wc_game.lpszClassName = L"Game";
    wc_game.style = CS_HREDRAW | CS_VREDRAW;
    wc_game.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc_game);
    hwndGameWindow = CreateWindowEx(
        0,                                
        L"Game",
        L"Game",
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL,       
        NULL,       
        hInstance,  
        NULL        
    );
    if (hwndGameWindow == NULL)
    {
        return 1;
    }
    if (!DrawComponents(hwndSettingsWindow, hInstance)) {
        return 1;
    }
    ShowWindow(hwndSettingsWindow, nCmdShow);
    UpdateWindow(hwndSettingsWindow);
    MSG msg;
    HANDLE data_thread = CreateThread(nullptr, 0, GetData, NULL, 0, nullptr);
    if (data_thread == NULL) {
        return 1;
    }
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    CloseHandle(data_thread);
    return (int)msg.wParam;
}

DWORD WINAPI InitilizeTargets(LPVOID lpParam) {
    target t1(3, 150, 150, 600);
    t1.setImage(imageList[3 - 3]);
    targets.push_back(t1);
    return 0;
}

DWORD WINAPI GetData(LPVOID lpParam) {
    int port = 8888;
    const char* ipAddressStr = "192.168.150.2";
    POINTFLOAT radianPoint;
    Connection network(ipAddressStr,9998);
    network.Connect();
    while (true)
    {
        if (network.NextXY(radianPoint))
        {
            currentAngles = { radianPoint.x * 180/(float)M_PI , radianPoint.y * 180 / (float)M_PI };
            if (CenterCalibrated) {
                currentAngles.x -= centerXAngle;
                currentAngles.y -= centerYAngle;
            }
            /*if (isplaying) {
                scope_x -= currentAngles.x;
                scope_y -= currentAngles.y;
            }*/
            if (EnableStopX) {
                minXAngle = currentAngles.x;
                std::wstring floatString = std::to_wstring(minXAngle);
                SetWindowText(hwndXTexbox, floatString.c_str());
            } 
            if (EnableStopY) {
                minYAngle = currentAngles.y;
                std::wstring floatString = std::to_wstring(minYAngle);
                SetWindowText(hwndYTexbox, floatString.c_str());
            } 
            if (EnableStopCenter) {
                centerYAngle = currentAngles.y;
                centerXAngle = currentAngles.x;
            }
        }
    }
    return 0;
}

void RestartGame(HWND hwnd) {
    isplaying = true;
    scope.reset();
    score = 0;
    current_target_number = -1;
    bullet_number = 0;
    for (int i = 0; i < max_bullets_to_draw; i++) bullets[i] = Bullet();
    SetTimer(hwnd, IDC_DRAWTIMER_ID, 0, NULL);
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
    RECT rect = { WINDOW_WIDTH - 300, 0, WINDOW_WIDTH - 298, WINDOW_HEIGHT };
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
}

void DrawScope(HDC hdc) {
    Graphics graphics(hdc); 
    UINT width = scope_png->GetWidth();
    UINT height = scope_png->GetHeight();
    graphics.DrawImage(scope_png, scope.getX(), scope.getY(), width, height);
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
    RECT text_rect = { WINDOW_WIDTH - 300,20,WINDOW_WIDTH,70 };
    string temp = "Score: ";
    temp += to_string(fps);
    temp += "\0\0";
    wstring text(temp.begin(), temp.end());
    DrawTextW(hdc, text.c_str(), -1, &text_rect, DT_CENTER);
}

void DrawRestartButton(HDC hdc) {
    //рисование кнопки относительно рабочей области а не по координатам через GetClientRect
    SetBkMode(hdc, TRANSPARENT);
    RECT restart_game = { WINDOW_WIDTH - 278,500,WINDOW_WIDTH - 20,560 };
    //RECT restart_game_text = { 620,555,880,580 };
    HBRUSH hBrush = CreateSolidBrush(RGB(181, 181, 181));
    FillRect(hdc, &restart_game, hBrush);
    DeleteObject(hBrush);
    const WCHAR text[] = L"Restart";
    int textSize = ARRAYSIZE(text);
    //DrawTextW(hdc, text, textSize, &restart_game_text, DT_CENTER | DT_VCENTER);
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

LRESULT CALLBACK Game_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Converter converter(600, 600, 20.0f, 20.0f);
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        KillTimer(hwnd, IDC_FPSTIMER_ID);
        KillTimer(hwnd, IDC_DRAWTIMER_ID);
        KillTimer(hwnd, 5);
        return 0;

    case WM_CREATE:
    {
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
        fps++;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillBackground(hwnd, hdcBuffer);
        DrawDivider(hdcBuffer);
        DrawScoreText(hdcBuffer);
        if (isplaying) {
            if (current_target != NULL) {
                DrawTarget(hdcBuffer);
            }
            //DrawBullets(hdcBuffer);
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
        case IDC_FPSTIMER_ID:
        {
            POINTFLOAT nextPoint;
            POINT newCenter;
            nextPoint = currentAngles;
            newCenter = converter.ToCoord(nextPoint);
            scope.move_by_angles(currentAngles);    
            if (isplaying) {
                //Shoot();
                CalculateScore();
            }
            DoubleBuff(hwnd);
            //fps++;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        case 5:
        {
            v.push_back(fps);
            fps = 0;
        }
        case IDC_DRAWTIMER_ID:
        {
            KillTimer(hwnd, IDC_DRAWTIMER_ID);
            SwitchTarget();
            if(isplaying)
                SetTimer(hwnd, IDC_DRAWTIMER_ID, 1000 * current_target->ttl, NULL);
        }
        default:
            break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int DrawComponents(HWND hwnd, HINSTANCE hInstance) {
    hLabelX = CreateWindowEx(0, L"STATIC", L"Colibrating X", WS_CHILD | WS_VISIBLE, 100, 10, 200, 20, hwnd, NULL, NULL, NULL);
    hwndXTexbox = CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_CENTER,
        100, 30, 200, 45,
        hwnd,
        (HMENU)IDC_XCALIBRATING_TEXTBOX,
        hInstance,
        NULL
    );
    SendMessage(hwndXTexbox, WM_SETFONT, (WPARAM)hTextBoxFont, TRUE);
    SendMessage(hwndXTexbox, EM_SETREADONLY, TRUE, 0);
    hwndButtonStartCalibratingX = CreateWindow(
        L"BUTTON",
        L"Start calibrating X",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        100, 80, 95, 40,
        hwnd,
        (HMENU)IDC_XSTARTCALIBRATING_BUTTON,
        hInstance,
        NULL
    );
    hwndButtonStopCalibratingX = CreateWindow(
        L"BUTTON",
        L"Stop calibrating X",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        205, 80, 95, 40,
        hwnd,
        (HMENU)IDC_XSTOPCALIBRATING_BUTTON,
        hInstance,
        NULL
    );
    EnableWindow(hwndButtonStopCalibratingX, FALSE);

    hLabelY = CreateWindowEx(0, L"STATIC", L"Calibrating Y", WS_CHILD | WS_VISIBLE, 100, 130, 200, 20, hwnd, NULL, NULL, NULL);
    hwndYTexbox = CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_CENTER,
        100, 150, 200, 45,
        hwnd,
        (HMENU)IDC_YCALIBRATING_TEXTBOX,
        hInstance,
        NULL
    );
    SendMessage(hwndYTexbox, WM_SETFONT, (WPARAM)hTextBoxFont, TRUE);
    SendMessage(hwndYTexbox, EM_SETREADONLY, TRUE, 0);
    hwndButtonStartCalibratingY = CreateWindow(
        L"BUTTON",
        L"Start calibrating Y",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        100, 200, 95, 40,
        hwnd,
        (HMENU)IDC_YSTARTCALIBRATING_BUTTON,
        hInstance,
        NULL
    );
    hwndButtonStopCalibratingY = CreateWindow(
        L"BUTTON",
        L"Stop calibrating Y",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        205, 200, 95, 40,
        hwnd,
        (HMENU)IDC_YSTOPCALIBRATING_BUTTON,
        hInstance,
        NULL
    );
    EnableWindow(hwndButtonStopCalibratingY, FALSE);

    hLabelSreen = CreateWindowEx(0, L"STATIC", L"Choose window size", WS_CHILD | WS_VISIBLE, 100, 250, 200, 20, hwnd, NULL, NULL, NULL);
    hComboBox = CreateWindowEx(
        0,
        L"ComboBox",
        NULL,
        CBS_DROPDOWN | WS_VSCROLL | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        100, 270, 200, 90,
        hwnd,
        (HMENU)IDC_RESOLUTION_DROPLIST,
        hInstance,
        NULL
    );
    for (const wchar_t* resolution : resolutions) {
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)resolution);
    }

    hwndButtonStartCalibratingCenter = CreateWindow(
        L"BUTTON",
        L"Start calibrating center",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        100, 300, 95, 50,
        hwnd,
        (HMENU)IDC_STARTCALIBRATINGCENTER_BUTTON,
        hInstance,
        NULL
    );
    hwndButtonStopCalibratingCenter = CreateWindow(
        L"BUTTON",
        L"Stop calibrating center",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        205, 300, 95, 50,
        hwnd,
        (HMENU)IDC_STOPCALIBRATINGCENTER_BUTTON,
        hInstance,
        NULL
    );
    EnableWindow(hwndButtonStopCalibratingCenter, FALSE);
    hwndButtonStartGame = CreateWindow(
        L"BUTTON",
        L"Start game",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        100, 360, 200, 30,
        hwnd,
        (HMENU)IDC_STARTGAME_BUTTON,
        hInstance,
        NULL
    );
    return 1;
}

void SetResolution(const wchar_t* resolution) {
    wstring wstr(resolution);
    int pos = wstr.find(L"x");
    if (pos != -1) {
        wstring width = wstr.substr(0, pos);
        wstring height = wstr.substr(pos + 1);
        WINDOW_WIDTH = stoi(width);
        WINDOW_HEIGHT = stoi(height);
    }
    else {
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
        WINDOW_WIDTH = workArea.right - workArea.left;
        WINDOW_HEIGHT = workArea.bottom - workArea.top;
    }
    SetWindowPos(
        hwndGameWindow,
        NULL,
        0, 0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SWP_NOMOVE | SWP_NOZORDER
    );
}

LRESULT CALLBACK Settings_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {   
        PostQuitMessage(0);
        return 0;
    }
    case WM_CREATE:
    {
        hTextBoxFont = CreateFontW(39, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, L"Arial");
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        if ((HWND)lParam == hLabelX || (HWND)lParam == hLabelY || (HWND)lParam == hLabelSreen)
        {
            return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        break;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_PAINT:
    {
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_XSTARTCALIBRATING_BUTTON:
        {  
            EnableStopX = true;
            XCalibrated = false;
            EnableWindow(hwndButtonStopCalibratingX, TRUE);
            break;
        }
        case IDC_YSTARTCALIBRATING_BUTTON:
        {
            EnableStopY = true;
            YCalibrated = false;
            EnableWindow(hwndButtonStopCalibratingY, TRUE);
            break;
        }
        case IDC_XSTOPCALIBRATING_BUTTON:
        {
            EnableStopX = false;
            XCalibrated = true;
            EnableWindow(hwndButtonStopCalibratingX, FALSE);
            break;
        }
        case IDC_YSTOPCALIBRATING_BUTTON:
        {
            EnableStopY = false;
            YCalibrated = true;
            EnableWindow(hwndButtonStopCalibratingY, FALSE);
            break;
        }
        case IDC_STARTCALIBRATINGCENTER_BUTTON:
        {
            EnableStopCenter = true;
            EnableWindow(hwndButtonStopCalibratingCenter, TRUE);
            break;
        }
        case IDC_STOPCALIBRATINGCENTER_BUTTON:
        {
            CenterCalibrated = true;
            EnableStopCenter = false;
            EnableWindow(hwndButtonStopCalibratingCenter, FALSE);
            break;
        }
        case IDC_STARTGAME_BUTTON:
        {
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            if (XCalibrated && YCalibrated && CenterCalibrated && selectedIndex != -1) {
                isplaying = true;
                ShowWindow(hwndSettingsWindow, SW_HIDE);
                SetTimer(hwndGameWindow, IDC_FPSTIMER_ID, 1000 / FPS_LIMIT, NULL);
                SetTimer(hwndGameWindow, IDC_DRAWTIMER_ID, 0, NULL);
                SetTimer(hwndGameWindow, 5, 1000, NULL);
                if (hTargetsThread != NULL) {
                    WaitForSingleObject(hTargetsThread, INFINITE);
                    CloseHandle(hTargetsThread);
                    hTargetsThread = NULL;
                }
                scope.setMaxXAngle(minXAngle);
                scope.setMaxYAngle(minYAngle);
                scope.setMinXAngle(minXAngle);
                scope.setMinYAngle(minYAngle);
                scope.setCenterXAngle(centerXAngle);
                scope.setCenterYAngle(centerYAngle);
                ShowWindow(hwndGameWindow, SW_SHOWDEFAULT);                
                SetResolution(resolutions[selectedIndex]);
                RECT clientRect;
                GetClientRect(hwndGameWindow, &clientRect);
                scope.setWorkingAreaWidth(WINDOW_WIDTH);
                scope.setWorkingAreaHeight(clientRect.bottom - clientRect.top);
                scope.reset();
            }
            else {
                MessageBox(hwndSettingsWindow, L"First callbrate X, Y, Center, and choose screen resolution", L"Error", MB_ICONWARNING | MB_OK);
            }
            break;
        }
        }
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}