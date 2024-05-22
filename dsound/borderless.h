#pragma once
namespace borderless {
    static bool bBorderlessWindowed = true;
    static HWND GameHWND = NULL;

    static std::tuple<int, int, int, int> beforeCreateWindow(int nWidth, int nHeight)
    {
        // fix the window to open at the center of the screen...
        HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX info = { sizeof(MONITORINFOEX) };
        GetMonitorInfo(monitor, &info);
        DEVMODE devmode = {};
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
        DWORD DesktopX = devmode.dmPelsWidth;
        DWORD DesktopY = devmode.dmPelsHeight;

        int newWidth = nWidth;
        int newHeight = nHeight;

        int WindowPosX = (int)(((float)DesktopX / 2.0f) - ((float)newWidth / 2.0f));
        int WindowPosY = (int)(((float)DesktopY / 2.0f) - ((float)newHeight / 2.0f));

        SetProcessDPIAware();

        return std::make_tuple(WindowPosX, WindowPosY, newWidth, newHeight);
    }

    static void afterCreateWindow()
    {
        LONG lStyle = GetWindowLong(GameHWND, GWL_STYLE);
        if (bBorderlessWindowed)
            lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        else
        {
            lStyle |= (WS_MINIMIZEBOX | WS_SYSMENU);
        }
        SetWindowLong(GameHWND, GWL_STYLE, lStyle);
        SetWindowPos(GameHWND, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    static BOOL WINAPI AdjustWindowRect_Hook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
    {
        DWORD newStyle = 0;

        if (!bBorderlessWindowed)
            newStyle = WS_CAPTION;
        else
        {
            afterCreateWindow();
            return AdjustWindowRect(lpRect, GetWindowLong(GameHWND, GWL_STYLE), bMenu);
        }

        return AdjustWindowRect(lpRect, newStyle, bMenu);
    }

    static BOOL WINAPI CenterWindowPosition(int nWidth, int nHeight)
    {
        // fix the window to open at the center of the screen...
        HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX info = { sizeof(MONITORINFOEX) };
        GetMonitorInfo(monitor, &info);
        DEVMODE devmode = {};
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
        DWORD DesktopX = devmode.dmPelsWidth;
        DWORD DesktopY = devmode.dmPelsHeight;

        int newWidth = nWidth;
        int newHeight = nHeight;


        int WindowPosX = (int)(((float)DesktopX / 2.0f) - ((float)newWidth / 2.0f));
        int WindowPosY = (int)(((float)DesktopY / 2.0f) - ((float)newHeight / 2.0f));

        return SetWindowPos(GameHWND, 0, WindowPosX, WindowPosY, newWidth, newHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    static HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
    {
        auto [WindowPosX, WindowPosY, newWidth, newHeight] = beforeCreateWindow(nWidth, nHeight);
        GameHWND = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, WindowPosX, WindowPosY, newWidth, newHeight, hWndParent, hMenu, hInstance, lpParam);
        afterCreateWindow();
        return GameHWND;
    }


    static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
    {
        BOOL res = SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
        if (bBorderlessWindowed)
        {
            afterCreateWindow();
            RECT rect;
            GetClientRect(hWnd, &rect);
            CenterWindowPosition(rect.right, rect.bottom);
            return TRUE;
        }
        return res;

    }

}