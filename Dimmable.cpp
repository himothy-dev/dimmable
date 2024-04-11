// Dimmable.cpp : Defines the entry point for the application.

#include "framework.h"
#include "Dimmable.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "Resource.h"
#include <commctrl.h>
#include <psapi.h>
#pragma comment(lib, "comctl32.lib")


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst; // current instance
float opacity = 0.5f;

// Buffer to store the window name
static WCHAR windowName[256];
HWND selectedWindowHandle;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateFullScreenWindow(HINSTANCE);


HWND getTargetWindow()
{
    if (selectedWindowHandle)
    {
        return selectedWindowHandle;
    }

    HWND targetWnd = FindWindow(NULL, windowName);
    if (!targetWnd)
    {
        // TODO Handle error: target window not found
        return NULL;
    }

    selectedWindowHandle = targetWnd;

    return targetWnd;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    WCHAR buffer[256];

    // Buffer to store the window class name
    //WCHAR className[256];

    // Get PID from window handle

    /*DWORD dwPID;
    GetWindowThreadProcessId(hWnd, &dwPID);

    // Get executable name from PID
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    WCHAR exeName[MAX_PATH];

    if (hProcess != NULL) {
        if (GetModuleFileNameEx(hProcess, NULL, exeName, MAX_PATH)) {
			// Get the file name from the full path
			WCHAR* p = wcsrchr(exeName, L'\\');
            if (p) {
				wcscpy_s(exeName, p + 1);
			}
           
        }
    }*/

    if (IsWindowVisible(hWnd) && GetWindowText(hWnd, buffer, sizeof(buffer) / sizeof(WCHAR))) {

        // Get the window class name
        //GetClassName(hWnd, className, sizeof(className) / sizeof(WCHAR));

        if (wcscmp(buffer, L"Dimmable") != 0) { // Exclude the window with the title "Dimmable"
            SendDlgItemMessage((HWND)lParam, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)buffer);
            /*reinterpret_cast<LPARAM>((std::to_wstring(dwPID) + L" " + std::wstring(exeName) + L" " + className).c_str())*/
            
        }
    }

    // Automatically select the first item
    SendDlgItemMessage((HWND)lParam, IDC_COMBO1, CB_SETCURSEL, 0, 0);

    return TRUE;
}

// Message handler for the input box.
INT_PTR CALLBACK InputBoxProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        // Message box 
    {
        if(hInst == NULL){
			// Message box
			MessageBox(NULL, L"hInst is NULL", L"Error", MB_OK | MB_ICONERROR);
		}
        HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DIMMABLE));
        HICON hIconSmall = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));

        if (hIcon)
        {
            SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }else{
        	// Message box
			MessageBox(NULL, L"LoadIcon() failed", L"Error", MB_OK | MB_ICONERROR);
        }
    }

    {
        // Get a handle to the slider control
        HWND hOpacitySlider = GetDlgItem(hDlg, IDC_SLIDER1);

        // Set the range of the slider to be 1 to 10
        SendMessage(hOpacitySlider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 10));

        // Set the position of the slider to the current opacity value
        SendMessage(hOpacitySlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(opacity * 10));
    }
        RECT rc;
        int xPos, yPos;
        GetWindowRect(hDlg, &rc);
        xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
        yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
        SetWindowPos(hDlg, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


        EnumWindows(EnumWindowsProc, (LPARAM)hDlg);

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            // When the user clicks "OK", get the selected window name from the combo box
            int index = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
            SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETLBTEXT, index, (LPARAM)windowName);

            {
                // Get a handle to the slider control
                HWND hOpacitySlider = GetDlgItem(hDlg, IDC_SLIDER1);

                // Get the current position of the slider
                int pos = SendMessage(hOpacitySlider, TBM_GETPOS, 0, 0);

                // Convert the position to a float in the range 0.1 to 1.0
                opacity = pos / 10.0f;
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            // When the user clicks "Cancel" or "Exit", close the application
            PostQuitMessage(0);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        exit(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    hInst = hInstance;
    DialogBox(hInst, MAKEINTRESOURCE(IDD_INPUTBOX), NULL, InputBoxProc);

    // Create a simple visible window
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("VisibleWindowClass");
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIMMABLE));

    RegisterClass(&wc);
    HWND visibleWindow = CreateWindow(wc.lpszClassName, TEXT("Dimmable"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, NULL, NULL, hInstance, NULL);
    ShowWindow(visibleWindow, SW_SHOWMINIMIZED);

    HWND hwnd;
    MSG msg;

    hwnd = CreateFullScreenWindow(hInstance);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    SetWindowLong(hwnd, GWL_EXSTYLE,
    GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), (int)(opacity * 255), LWA_ALPHA);

    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC			hdc;
    PAINTSTRUCT	ps;
    RECT		rect;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);

            EndPaint(hwnd, &ps);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_TIMER:
        {
            // Find the target window
            HWND targetWnd = getTargetWindow();
            if (targetWnd)
            {
                // Get the target window's position and size
                RECT rect;
                GetWindowRect(targetWnd, &rect);

                // Move and resize the overlay window to match the target window's position and size
                SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
            }
        }
    }


    return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND CreateFullScreenWindow(HINSTANCE hInstance)
{
    static TCHAR szAppName[] = TEXT("Overlay");

    HWND targetWnd = getTargetWindow();
    if (!targetWnd)
    {
        // TODO Handle error: target window not found
        return NULL;
    }

    HWND		hwnd = targetWnd; // GetDesktopWindow();
    WNDCLASS	wndclass;

    HWND hDesktop = GetDesktopWindow();
    RECT windowShape;
    GetWindowRect(hwnd, &windowShape);

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_DIMMABLE));
    wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi))
        return NULL;

    HWND win_hwnd = CreateWindowEx(WS_EX_TRANSPARENT, szAppName,
        NULL,
        WS_POPUP | WS_VISIBLE,

        windowShape.left,
        windowShape.top,
        windowShape.right - windowShape.left,
        windowShape.bottom - windowShape.top,

        hwnd, NULL, hInstance, 0);

    // Make the window always on top
    SetWindowPos(win_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    
    // Set a timer to update the overlay window's position and size
    SetTimer(win_hwnd, 1, 10, NULL); // 10 ms interval

    return win_hwnd;
}
