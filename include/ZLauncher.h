#define WINVER 0x0A00
#define __WIN32_WINNT WINVER
#define JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE 0x00002000

#define UNICODE
#define _UNICODE

#include <string>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <shlwapi.h>
#include <tchar.h>

#include "Process.h"
#include "Utils.h"
#include "VideoPlayback.h"

using namespace std;

#ifdef UNICODE
typedef wstring str;
typedef wstringstream sstream;
#else
typedef string str;
typedef stringstream sstream;
#endif // UNICODE

#define ERR_SUCCESS             0
#define ERR_CONFIG              1
#define ERR_ELEVATION           2
#define ERR_GAME                3
#define ERR_SPEEDHACK           4
#define ERR_STEAM_ELEVATION     5
#define ERR_STEAM_GAME          6

#define STEAM_RUNNING          -1
#define STEAM_RUNNING_ELEVATED -2

#define c(s) Utils::chars(s)

#define VP_LOG_CHECK_INTERVAL   30

#ifndef ZLAUNCHER_H
#define ZLAUNCHER_H

class ZLauncher
{
    public:
        ZLauncher(int argc, TCHAR* argv[]);

        int launch();
        int launchWithSteam();

        bool parseConfig();

        bool launchGame(HANDLE job);
        bool launchSpeedhack(HANDLE job);

        static bool restartElevated(bool launched = true);

        static void wait();

        static void showError();

        static str getConfigFile();
};

#endif // ZLAUNCHER_H
