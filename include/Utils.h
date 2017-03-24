using namespace std;

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

        static str filename(str path);

        static TCHAR* chars(str s);

        static wstring s2ws(string str);
        static string ws2s(wstring wstr);
};

#endif // UTILS_H
