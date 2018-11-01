/*
    MIT License

    Copyright (c) 2018 PS
    GitHub: https://github.com/ClnViewer/Monitor-Power-Off
    Info: very small testing utils to hardware monitor power set green mode (off)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sub license, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#define UNICODE
#define _UNICODE
#define _WIN32_IE 0x301
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <commctrl.h>

#define WIN_LABEL  1001
#define WIN_TRACK  1002
#define WIN_BUTTON 1003
#define WIN_TIMER  1004

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
wchar_t szClassName[] = L"MonitorPowerOffApp";
wchar_t textTracker[] = L"     Выключение питания монитора через %ld секунд";
HWND hTrack, hProg, hLabel, hBtn;
LRESULT pos = 0L, cnt = 0L;

void __UpdateControls(void)
{
    LRESULT ppos;
    wchar_t buf[256] = {0};

    if (
        (!hLabel) ||
        ((ppos = SendMessageW(hTrack, TBM_GETPOS, 0, 0)) <= -1)
    )
        return;

    pos = ppos;
    wsprintfW(buf, textTracker, pos);
    SetWindowTextW(hLabel, buf);
}

void __CreateProgressBar(HWND hwnd)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);

    hProg = CreateWindowEx(
                0, PROGRESS_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                0, 30, 300, 30, hwnd, (HMENU)WIN_TIMER, NULL, NULL);

    SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, pos));
    SendMessage(hProg, PBM_SETSTEP, 1, 0);
    SendMessage(hProg, PBM_SETPOS, 0, 0);
    SetTimer(hwnd, WIN_TIMER, 1000, NULL);
}

void __CreateControls(HWND hwnd)
{
    INITCOMMONCONTROLSEX icex;
    HGDIOBJ hfont = GetStockObject(DEFAULT_GUI_FONT);

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    hLabel = CreateWindowW(
               L"Static", L"0",
               WS_CHILD | WS_VISIBLE,
               0, 0, 300, 30, hwnd, (HMENU)WIN_LABEL, NULL, NULL);

    hTrack = CreateWindowW(
                 TRACKBAR_CLASSW, L"Trackbar Control",
                 WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                 0, 30, 300, 30, hwnd, (HMENU)WIN_TRACK, NULL, NULL);

    hBtn   = CreateWindowW(
                 L"Button", L"Выключить!",
                 WS_VISIBLE | WS_CHILD,
                 0, 60, 300, 25, hwnd, (HMENU) WIN_BUTTON, NULL, NULL);

    SendMessageW(hTrack, TBM_SETRANGE,  TRUE, MAKELONG(0, 60));
    SendMessageW(hTrack, TBM_SETPAGESIZE, 0,  10);
    SendMessageW(hTrack, TBM_SETTICFREQ, 5, 0);
    SendMessageW(hTrack, TBM_SETPOS, TRUE, 5);
    SendMessageW(hLabel, WM_SETFONT, (LPARAM)hfont, true);
    SendMessageW(hBtn,   WM_SETFONT, (LPARAM)hfont, true);

    __UpdateControls();
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEXW wincl;

    (void) hPrevInstance;
    (void) lpszArgument;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassExW (&wincl))
    {
        MessageBoxW(NULL,
                    L"I'm trouble :)!",
                    L"Выключение питания монитора",
                    0U);
        return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowExW (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               L"Выключение питания монитора",
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               310,                 /* The programs width */
               125,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    if (!hwnd)
    {
        MessageBoxW(NULL,
                    L"I'm cracked :) !",
                    L"Выключение питания монитора",
                    0U);
        return 0;
    }

    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        __CreateControls(hwnd);
        break;
    }
    case WM_HSCROLL:
    {
        __UpdateControls();
        break;
    }
    case WM_TIMER:
    {
        SendMessage(hProg, PBM_STEPIT, 0, 0);
        cnt++;

        if (cnt == pos)
        {
            KillTimer(hwnd, WIN_TIMER);
            SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, MAKELPARAM(2,0));
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        break;
    }
    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
        case WIN_BUTTON:
        {
            if (cnt != 0L)
                break;

            __CreateProgressBar(hwnd);
            break;
        }
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        KillTimer(hwnd, WIN_TIMER);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
