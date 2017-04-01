#include "Utils.h"

str Utils::getCurrentDir()
{
    TCHAR dir[MAX_PATH];
    GetCurrentDirectory(sizeof(dir), (LPTSTR) &dir);
    str sdir(dir);
    sdir += L'\\';
    return sdir;
}

str Utils::getConfigString(str section, str key, str def)
{
    TCHAR buffer[512];
    GetPrivateProfileString(section.c_str(), key.c_str(), def.c_str(), (LPTSTR) &buffer, sizeof(buffer), (Utils::getCurrentDir() + ZLauncher::getConfigFile()).c_str());

    if(*buffer == 0) return def;

    return str(buffer);
}

bool Utils::getConfigBool(str section, str key, bool def)
{
    str s = Utils::getConfigString(section, key, def ? L"true" : L"false");

    if(s.empty()) return def;

    return tolower(*s.begin()) == L't';
}

str Utils::readReg(HKEY hive, str path, str key, str def)
{
    HKEY hKey = NULL;
    LPBYTE buffer[512];
    DWORD sz = sizeof(buffer);

    if(RegOpenKeyEx(hive, path.c_str(), NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, key.c_str(), NULL, NULL, (LPBYTE) &buffer, &sz) == ERROR_SUCCESS)
        {
            if(*buffer == 0) return def;

            return str(reinterpret_cast<TCHAR*>(buffer));
        }
        else
        {
            ZLauncher::showError();
        }
    }
    else
    {
        ZLauncher::showError();
    }

    RegCloseKey(hKey);
    return def;
}

void Utils::attachWindow(HWND child, HWND parent)
{
    SetParent(child, parent);
    DWORD style = GetWindowLong(child, GWL_STYLE);
    style &= ~(WS_POPUP | WS_CAPTION);
    style |= WS_CHILD;
    SetWindowLong(child, GWL_STYLE, style);
    RECT rc;
    GetClientRect(parent, &rc);
    MoveWindow(child, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
    UpdateWindow(parent);
}

HWND Utils::waitForWindow(LPCWSTR cls, LPCWSTR title)
{
    HWND hwnd = 0;
    while(!IsWindow(hwnd))
    {
        hwnd = FindWindow(cls, title);
        Sleep(50);
    }
    return hwnd;
}

HWND Utils::waitForChildWindow(HWND parent, LPCWSTR cls, LPCWSTR title)
{
    HWND hwnd = 0;
    while(!IsWindow(hwnd))
    {
        hwnd = FindWindowEx(parent, NULL, cls, title);
        Sleep(50);
    }
    return hwnd;
}

long long Utils::getTimeMS()
{
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
        return GetTickCount();
    }
}

long long Utils::getTime()
{
    return Utils::getTimeMS() / 1000LL;
}

str Utils::filename(str path)
{
    size_t pos = path.find_last_of(L"/\\");
    if(pos == str::npos) return path;
    return path.substr(pos + 1);
}

TCHAR* Utils::chars(str s)
{
    return const_cast<TCHAR *>(s.c_str());
}

std::wstring Utils::s2ws(std::string str)
{
    return std::wstring(str.begin(), str.end());
}
std::string Utils::ws2s(std::wstring wstr)
{
    return std::string(wstr.begin(), wstr.end());
}
