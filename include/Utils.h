#ifndef UTILS_H
#define UTILS_H

#include "ZLauncher.h"

class Utils
{
    public:
        static str getCurrentDir();

        static str getConfigString(str section, str key, str def = str());
        static bool getConfigBool(str section, str key, bool def = false);

        static str readReg(HKEY hive, str path, str key, str def = str());

        static void attachWindow(HWND child, HWND parent);
        static HWND waitForWindow(LPCWSTR cls, LPCWSTR title);
        static HWND waitForChildWindow(HWND parent, LPCWSTR cls, LPCWSTR title);

        static long long getTimeMS();
        static long long getTime();

        static str filename(str path);

        static TCHAR* chars(str s);

        static std::wstring s2ws(std::string str);
        static std::string ws2s(std::wstring wstr);
};

#endif // UTILS_H
