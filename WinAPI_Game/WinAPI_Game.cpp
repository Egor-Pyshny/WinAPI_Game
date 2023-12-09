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
#include "Logger.h"
#include "Target.h"
#include <queue>
#include "Bullet.h"
#include <windowsx.h>
#include <cmath>
#include <cwchar>
#include <iostream>
#include <random>
#include "Connection.h"
#include "Converter.h"

using namespace Gdiplus;
using namespace std;

LRESULT CALLBACK Game_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Settings_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int DrawComponentsSettings(HWND hwnd, HINSTANCE hInstance);
int DrawComponentsGame(HWND hwnd, HINSTANCE hInstance);
void InitializePhotos();
void PlaceTargetsRandom();
unsigned long long GetValue(HKEY hKey);
void SetNewValue(HKEY hKey, DWORDLONG data);
HKEY OpenOrCreateKey(HKEY section, LPCSTR keyPath);
DWORD WINAPI InitilizeDefaultTargets(LPVOID lpParam);
DWORD WINAPI GetData(LPVOID lpParam);

vector<int> v;
HWND hLabelX;
HWND hLabelY;
HWND hLabelTargetsAmount;
HWND hLabelResolution;
HWND hwndButtonStartCalibratingX;
HWND hwndButtonStopCalibratingX;
HWND hwndButtonStartCalibratingY;
HWND hwndButtonStopCalibratingY;
HWND hwndButtonStartCalibratingCenter;
HWND hwndButtonStopCalibratingCenter;
HWND hwndButtonStartGame;
HWND hwndButtonSettigs;
HWND hwndButtonRestartGame;
HWND hwndComboBox;
HWND hwndXTexbox;
HWND hwndYTexbox;
HWND hwndTargetsAmountTexbox;
HWND hwndGameWindow;
HWND hwndSettingsWindow;
HANDLE hTargetsThread;
HANDLE hDataThread;
HFONT hTextBoxFont;
Bitmap* imageList[target::types];
Bullet bullets[MAX_BULLETS];
Scope scope;
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;
HBITMAP hOldBitmap = NULL;
Bitmap* scope_png = NULL;
vector<target> targets;
target* current_target = NULL;
queue<Point> points;
POINTFLOAT currentAngles;
Logger* logger_angles;
Logger* logger_coords;
Logger* logger_targets;
HKEY hkey;
unsigned long long game_id;
bool EnableStopX = false;
bool EnableStopY = false;
bool EnableStopCenter = false;
bool XCalibrated = false;
bool YCalibrated = false;
bool CenterCalibrated = false;
const wchar_t* resolutions[] = IDC_RESOLUTIONLIST;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
const int FPS_LIMIT = 64;
int score = 0;
int current_target_number = 0;
int bullet_number = 0;
int targets_amount = 5;
bool isplaying = false;
float XAngle = 0;
float YAngle = 0;
float centerXAngle = 0;
float centerYAngle = 0;
int fps = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    InitializePhotos();
    hTargetsThread = CreateThread(nullptr, 0, InitilizeDefaultTargets, NULL, 0, nullptr);
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
    RECT newClientRect = { 0, 0, 400, 450 };
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
    hkey = OpenOrCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\MyCompany\\MyApp");
    DWORDLONG id = GetValue(hkey);
    if (id == -1) {
        SetNewValue(hkey, 0);
        game_id = 0;
    }
    else {
        game_id = id;
    }
    logger_angles = new Logger(ANGLES, game_id);
    logger_coords = new Logger(COORDS, game_id);
    logger_targets = new Logger(TARGETS, game_id);
    scope.setLogger(logger_coords);
    DrawComponentsSettings(hwndSettingsWindow, hInstance);
    ShowWindow(hwndSettingsWindow, nCmdShow);
    UpdateWindow(hwndSettingsWindow);
    MSG msg;
    hDataThread = CreateThread(nullptr, 0, GetData, NULL, 0, nullptr);
    if (hDataThread == NULL) {
        return 1;
    }
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    CloseHandle(hDataThread);
    return (int)msg.wParam;
}

DWORD WINAPI InitilizeDefaultTargets(LPVOID lpParam) {
    int targets_info[][2] = IDC_TARGETSLIST;
    for (int* target_info : targets_info) {
        target t(target_info[0], target_info[1]);
        t.setImage(imageList[target_info[0] - 3]);
        targets.push_back(t);
    }
    //target t1(3, 150, 150, 600);
    //t1.setImage(imageList[3 - 3]);
    //targets.push_back(t1);
    return 0;
}

HKEY OpenOrCreateKey(HKEY section, LPCSTR keyPath) {
    HKEY hKey;
    DWORD res;
    if (RegCreateKeyExA(section, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hKey, &res) == ERROR_SUCCESS) {
        if (res == REG_CREATED_NEW_KEY) {
            cout << "Key created" << endl;
        }
        else if (res == REG_OPENED_EXISTING_KEY) {
            cout << "Key is already created" << endl;
        }
    }
    else {
        return NULL;
    }
    return hKey;
}

unsigned long long GetValue(HKEY hKey) {
    DWORDLONG data;
    DWORD dataSize = sizeof(data);
    DWORD dataType;
    if (RegQueryValueEx(hKey, L"GAMEID", nullptr, &dataType, (BYTE*)(&data), &dataSize) == ERROR_SUCCESS) {
        if (dataType == REG_QWORD) {
            return data;
        }
    }
    return -1;
}

void SetNewValue(HKEY hKey, DWORDLONG data) {
    if (RegSetValueExW(hKey, L"GAMEID", 0, REG_QWORD, (BYTE*)(&data), sizeof(data)) == ERROR_SUCCESS) {
        cout << "Data is recorded" << endl;
    }
    else {
        cout << "Error" << endl;
    }
}

void PlaceTargetsRandom(){
    int targets_info[][2] = IDC_TARGETSLIST;
    int prev_x = -1;
    int prev_y = -1;
    int prev_target_width = -1;
    int prev_target_height = -1;
    int x;
    int y;
    int target_width;
    int target_height;
    bool flag;
    random_device random_device;
    mt19937 generator(random_device());
    for (int i = 0; i < targets.size(); i++) {
        target* t = &targets[i];
        flag = true;
        target_width = t->getWidth();
        target_height = t->getHeight();
        while (flag) {
            uniform_int_distribution<> distribution_x(0, WINDOW_WIDTH - 300 - target_width);
            uniform_int_distribution<> distribution_y(0, WINDOW_HEIGHT - 300 - target_height);
            x = distribution_x(generator);
            y = distribution_y(generator);
            if (prev_x != -1) {
                flag = ((x >= prev_x && x <= prev_x + prev_target_width) && (y >= prev_y && y <= prev_y + prev_target_height));
            }
            else {
                flag = false;
            }
        }
        t->setX(x);
        t->setY(y);
        prev_x = x;
        prev_y = y;
        prev_target_width = target_width;
        prev_target_height = target_height;
    }
    logger_targets->queues.targets_queue = &targets;
}

void GenerateTargets() {
    targets.clear();
    random_device random_device;
    mt19937 generator(random_device());
    uniform_int_distribution<> distribution_section(3, 5);
    uniform_int_distribution<> distribution_ttl(1, 5);
    for (int i = 0; i < targets_amount; i++) {
        int sections = distribution_section(generator);
        int ttl = distribution_ttl(generator);
        target t(sections, ttl);
        t.setImage(imageList[sections - 3]);
        targets.push_back(t);
    }
}

DWORD WINAPI GetData(LPVOID lpParam) {
    //int port = 8888;
    //const char* ipAddressStr = "192.168.150.2";
    //POINTFLOAT radianPoint;
    //Connection network(ipAddressStr,9998);
    //network.setLogger(logger_angles);
    //bool connected = network.Connect();
    //if(connected){
    //    while (true)
    //    {
    //        if (network.NextXY(radianPoint))
    //        {
    //            currentAngles = { radianPoint.x * 180/(float)M_PI , radianPoint.y * 180 / (float)M_PI };
    //            if (CenterCalibrated) {
    //                currentAngles.x -= centerXAngle;
    //                currentAngles.y -= centerYAngle;
    //            }
    //            /*if (isplaying) {
    //                scope_x -= currentAngles.x;
    //                scope_y -= currentAngles.y;
    //            }*/
    //            if (EnableStopX) {
    //                minXAngle = currentAngles.x;
    //                std::wstring floatString = std::to_wstring(XAngle);
    //                SetWindowText(hwndXTexbox, floatString.c_str());
    //            } 
    //            if (EnableStopY) {
    //                minYAngle = currentAngles.y;
    //                std::wstring floatString = std::to_wstring(YAngle);
    //                SetWindowText(hwndYTexbox, floatString.c_str());
    //            } 
    //            if (EnableStopCenter) {
    //                centerYAngle = currentAngles.y;
    //                centerXAngle = currentAngles.x;
    //            }
    //        }
    //    }
    //}
    //else { 
    //    return 1; 
    //}
    return 0;
}

void RestartGame(HWND hwnd) {
    PlaceTargetsRandom();
    isplaying = true;
    scope.reset();
    score = 0;
    current_target_number = 0;
    bullet_number = 0;
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i] = Bullet();
    if (targets.size() > 0) {
        current_target = &targets[current_target_number];
        current_target_number++;
    }
    SetTimer(hwndGameWindow, IDC_DRAWTIMER_ID, 1000 * current_target->getTTL(), NULL);
}

void Shoot() {
    if (bullet_number <= MAX_BULLETS - 1) {
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
    graphics.DrawImage(current_target->getImage(), current_target->getX(), current_target->getY(), current_target->getWidth(), current_target->getWidth());
}

void DrawBullets(HDC hdc) {
    HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hRedBrush);
    for (int i = 0; i < MAX_BULLETS; i++) {
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
    temp += to_string(current_target_number-1);
    temp += "\0\0";
    wstring text(temp.begin(), temp.end());
    DrawTextW(hdc, text.c_str(), -1, &text_rect, DT_CENTER);
}

void DrawRestartButton(HDC hdc) {
    //рисование кнопки относительно рабочей области а не по координатам через GetClientRect
    SetBkMode(hdc, TRANSPARENT);
    RECT restart_game = { WINDOW_WIDTH - 280,500,WINDOW_WIDTH - 34,560 };
    //RECT restart_game_text = { 620,555,880,580 };
    HBRUSH hBrush = CreateSolidBrush(RGB(181, 181, 181));
    FillRect(hdc, &restart_game, hBrush);
    DeleteObject(hBrush);
    const WCHAR text[] = L"Restart";
    int textSize = ARRAYSIZE(text);
    //DrawTextW(hdc, text, textSize, &restart_game_text, DT_CENTER | DT_VCENTER);
}

void SwitchTarget() {
    if ((current_target_number < targets.size()) && isplaying) {
        current_target = &targets[current_target_number];
        current_target_number+=1;
        SetTimer(hwndGameWindow, IDC_DRAWTIMER_ID, 1000 * current_target->getTTL() + 1, NULL);
    }
    else {
        KillTimer(hwndGameWindow, IDC_DRAWTIMER_ID);
        logger_angles->finish();
        logger_coords->finish();
        logger_targets->finish();
        logger_angles->setGameId(game_id);
        logger_coords->setGameId(game_id);
        logger_targets->setGameId(game_id);
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

void ReleaseDoubleBuffer() {
    SelectObject(hdcBuffer, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
}

LRESULT CALLBACK Game_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY: {
        logger_angles->finish();
        logger_coords->finish();
        logger_targets->finish();
        logger_angles->setGameId(game_id);
        logger_coords->setGameId(game_id);
        logger_targets->setGameId(game_id);
        //выставить новый game_id после finish
        ShowWindow(hwndSettingsWindow, SW_SHOWNORMAL);
        KillTimer(hwnd, IDC_FPSTIMER_ID);
        KillTimer(hwnd, IDC_DRAWTIMER_ID);
        KillTimer(hwnd, 5);
        break;
    }
    case WM_SHOWWINDOW:
    {
        bool flag = (bool)wParam;
        if (flag) {
            scope.reset();
            current_target_number = 0;
            score = 0;
            bullet_number = 0;
            isplaying = true;
            if (targets.size() > 0) {
                current_target = &targets[current_target_number];
                current_target_number++;
            }
            SetTimer(hwndGameWindow, IDC_DRAWTIMER_ID, 1000 * current_target->getTTL() + 1, NULL);
            SetTimer(hwndGameWindow, IDC_FPSTIMER_ID, 15, NULL);
            SetTimer(hwndGameWindow, 5, 1000, NULL);
            return 0;
        }
        break;
    }
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
        DrawRestartButton(hdcBuffer);
        DrawScope(hdcBuffer);
        BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcBuffer, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        ReleaseDoubleBuffer();
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
                logger_angles->start();
                logger_coords->start();
                logger_targets->start();     
                ofstream file(GAMEINFOFILE);
                file << "[GAMEID" << game_id << "]";
                file << "{ ""resolution"":" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT \
                    << ", ""targetsAmount"":" << targets.size() \
                    << ", ""XAngle"":" << XAngle \
                    << ", ""YAngle"":" << YAngle \
                    << ", ""centerXAngle"":" << centerXAngle \
                    << ", ""centerYAngle"":" << centerYAngle << "\n";
                file << "[ENDGAME]";
                file.close();
                game_id++;
                SetNewValue(hkey, game_id);
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
            /*POINTFLOAT nextPoint;
            POINT newCenter;
            nextPoint = currentAngles;
            newCenter = converter.ToCoord(nextPoint);*/
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
            SwitchTarget();
        }
        default:
            break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int DrawComponentsSettings(HWND hwnd, HINSTANCE hInstance) {
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
    hLabelResolution = CreateWindowEx(0, L"STATIC", L"Choose window size", WS_CHILD | WS_VISIBLE, 100, 250, 200, 20, hwnd, NULL, NULL, NULL);
    
    hwndComboBox = CreateWindowEx(
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
        SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)resolution);
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

    hLabelTargetsAmount = CreateWindowEx(0, L"STATIC", L"Targets amount:", WS_CHILD | WS_VISIBLE, 100, 360, 200, 20, hwnd, NULL, NULL, NULL);
    
    hwndTargetsAmountTexbox = CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_CENTER,
        100, 380, 200, 20,
        hwnd,
        (HMENU)IDC_YCALIBRATING_TEXTBOX,
        hInstance,
        NULL
    );

    hwndButtonStartGame = CreateWindow(
        L"BUTTON",
        L"Start game",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
        100, 410, 200, 30,
        hwnd,
        (HMENU)IDC_STARTGAME_BUTTON,
        hInstance,
        NULL
    );
    return 0;
}

int DrawComponentsGame(HWND hwnd, HINSTANCE hInstance) {
    return 0;
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
        if ((HWND)lParam == hLabelX || (HWND)lParam == hLabelY || (HWND)lParam == hLabelResolution || (HWND)lParam == hLabelTargetsAmount)
        {
            return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));
        }
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
            int selectedIndex = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);
            if (XCalibrated && YCalibrated && CenterCalibrated && selectedIndex != -1) {
                int textLength = GetWindowTextLength(hwndTargetsAmountTexbox) + 1;
                /*if (textLength > 1) {
                    wchar_t* buffer = new wchar_t[textLength];
                    wchar_t* stopwcs;
                    GetWindowText(hwndTargetsAmountTexbox, buffer, textLength);
                    long l = wcstol(buffer, &stopwcs, 10);
                    if (stopwcs[0] != '\0' || l == 0 || l >100) {
                        MessageBox(hwndSettingsWindow, L"Not valid targets amount", L"Error", MB_ICONWARNING | MB_OK);
                        break;
                    }      
                    targets_amount = l;
                }
                else {
                    int res = MessageBox(hwndSettingsWindow, L"You dont fill in targets amount. It'll be set to default value", L"Error", MB_YESNO | MB_DEFBUTTON1);
                    if (res == IDYES) {
                        targets_amount = 0;
                    }
                    else {
                        break;
                    }
                }*/
                hwndGameWindow = CreateWindowEx(
                    0,
                    L"Game",
                    L"Game",
                    WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
                    CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                    NULL,
                    NULL,
                    (HINSTANCE)GetWindowLongPtr(hwndSettingsWindow, GWLP_HINSTANCE),
                    NULL
                );
                SetResolution(resolutions[selectedIndex]);
                /*if (targets_amount != 0) {
                    GenerateTargets();
                }*/
                PlaceTargetsRandom();
                isplaying = true;
                ShowWindow(hwndSettingsWindow, SW_HIDE);
                scope.setXAngle(XAngle);
                scope.setYAngle(YAngle);
                scope.setCenterXAngle(centerXAngle);
                scope.setCenterYAngle(centerYAngle);
                if (hTargetsThread != NULL) {
                    WaitForSingleObject(hTargetsThread, INFINITE);
                    CloseHandle(hTargetsThread);
                    hTargetsThread = NULL;
                }
                RECT clientRect;
                GetClientRect(hwndGameWindow, &clientRect);
                scope.setWorkingAreaWidth(WINDOW_WIDTH);
                scope.setWorkingAreaHeight(clientRect.bottom - clientRect.top);
                scope.reset();
                logger_angles->start();
                logger_coords->start();
                logger_targets->start();
                ofstream file(GAMEINFOFILE);
                file << "[GAMEID" << game_id << "]";
                file << "{ ""resolution"":" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT \
                    << ", ""targetsAmount"":" << targets.size() \
                    << ", ""XAngle"":" << XAngle \
                    << ", ""YAngle"":" << YAngle \
                    << ", ""centerXAngle"":" << centerXAngle \
                    << ", ""centerYAngle"":" << centerYAngle << "\n";
                file << "[ENDGAME]";
                file.close();
                game_id++;
                SetNewValue(hkey, game_id);
                ShowWindow(hwndGameWindow, SW_SHOWNORMAL);
                UpdateWindow(hwndGameWindow);
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