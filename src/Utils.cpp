#include "ZLauncher.h"

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

wstring Utils::s2ws(string str)
{
    return wstring(str.begin(), str.end());
}
string Utils::ws2s(wstring wstr)
{
    return string(wstr.begin(), wstr.end());
}
