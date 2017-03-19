#include "ZLauncher.h"

#include <iostream>

using namespace std;

int argsCount;
TCHAR** args;

HANDLE job;
PROCESS_INFORMATION pemu;
PROCESS_INFORMATION psh;

bool elevated, gameIsLaunched, launchedFromSteam;

str cfg = L"zlauncher.ini";

str cemu, cemu_path, game_path, speedhack_path, cemu_cmdline, steam_path, steam_gameid, steam_cmdline;
bool cemu_fullscreen, game_elevate, speedhack_enabled, steam_enabled, steam_elevate;

static void mb(LPTSTR msg, LPTSTR title = L"Error", UINT flags = MB_ICONERROR)
{
    MessageBox(NULL, msg, title, flags | MB_OK);
}

ZLauncher::ZLauncher(int argc, TCHAR* argv[])
{
    argsCount = argc; args = argv;

    elevated = Process::isElevated();

    job = CreateJobObject(NULL, NULL);
    if(job)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(job, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    }

    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            TCHAR* arg = argv[i];
            TCHAR* prev = argv[i - 1];

            if(StrCmpI(arg, L"-gamelaunched") == 0)
            {
                gameIsLaunched = true;
            }
            else
            {
                if(StrCmpI(prev, L"-config") == 0)
                {
                    cfg = arg;
                }
            }
        }
    }

    str parentName = Process::parentName();

    if(StrCmpI(c(parentName), L"Steam.exe") == 0)
    {
        launchedFromSteam = true;
    }
}

int ZLauncher::launch()
{
    if(!this->parseConfig()) return 1;

    int steam_res = ERR_SUCCESS;
    if(steam_enabled && !steam_path.empty()) steam_res = ZLauncher::launchWithSteam();

    if(steam_res >= ERR_SUCCESS) return steam_res;

    if(game_elevate && !elevated) return ZLauncher::restartElevated(false) ? ERR_SUCCESS : ERR_ELEVATION;

    if(!gameIsLaunched && !this->launchGame(elevated ? job : nullptr)) return ERR_GAME;

    if(elevated)
    {
        if(speedhack_enabled && !launchSpeedhack(job)) return ERR_SPEEDHACK;

        if(pemu.hProcess)
        {
            WaitForSingleObject(pemu.hProcess, INFINITE);
            CloseHandle(pemu.hProcess);
            CloseHandle(pemu.hThread);
        }
        else
        {
            if(HANDLE cemu = Process::find(L"Cemu.exe"))
            {
                WaitForSingleObject(cemu, INFINITE);
                CloseHandle(cemu);
            }
        }

        if(psh.hProcess)
        {
            CloseHandle(psh.hProcess);
            CloseHandle(psh.hThread);
        }
    }
    else
    {
        ZLauncher::restartElevated();
    }

    return ERR_SUCCESS;
}

int ZLauncher::launchWithSteam()
{
    if(steam_elevate)
    {
        HANDLE hSteam = Process::find(L"Steam.exe");
        if(hSteam)
        {
            if(launchedFromSteam && Process::isElevated(hSteam))
            {
                CloseHandle(hSteam);
                return STEAM_RUNNING_ELEVATED;
            }
            else
            {
                TerminateProcess(hSteam, 0);
                CloseHandle(hSteam);
                return Process::startElevated(steam_path, steam_cmdline, Utils::getCurrentDir(), nullptr, false) ? ERR_SUCCESS : ERR_STEAM_ELEVATION;
            }
        }
        else
        {
            return Process::startElevated(steam_path, steam_cmdline, Utils::getCurrentDir(), nullptr, false) ? ERR_SUCCESS : ERR_STEAM_ELEVATION;
        }
    }

    return Process::start(NULL, steam_path, steam_cmdline, Utils::getCurrentDir(), nullptr, false) ? ERR_SUCCESS : ERR_STEAM_GAME;
}

bool ZLauncher::parseConfig()
{
    cemu_path = Utils::getConfigString(L"cemu", L"path");
    game_path = Utils::getConfigString(L"game", L"path");

    cemu_cmdline = Utils::getConfigString(L"cemu", L"cmdline");
    cemu_fullscreen = Utils::getConfigBool(L"cemu", L"fullscreen", true);
    game_elevate = Utils::getConfigBool(L"game", L"elevate", false);

    speedhack_enabled = Utils::getConfigBool(L"speedhack", L"enabled", false);

    if(speedhack_enabled)
        speedhack_path = Utils::getConfigString(L"speedhack", L"path", L".\\speedhack.exe");

    steam_enabled = Utils::getConfigBool(L"steam", L"enabled", false);
    if(steam_enabled)
    {
        steam_elevate = Utils::getConfigBool(L"steam", L"elevate", false);
        steam_gameid = Utils::getConfigString(L"steam", L"gameid");
        steam_path = Utils::readReg(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", L"SteamExe");
        steam_cmdline = L"\"" + steam_path + L"\" \"steam://rungameid/" + steam_gameid + L"\"";
    }

    if(cemu_path.empty() || game_path.empty()) return false;

    cemu = cemu_path + L"\\Cemu.exe";

    str game_rpx = game_path + L"\\code\\U-King.rpx";

    cemu_cmdline = L"\"" + cemu + L"\" " + (cemu_fullscreen ? L"-f " : L"") + cemu_cmdline + L" -g \"" + game_rpx + L"\"";

    return true;
}

bool ZLauncher::launchGame(HANDLE job)
{
    return Process::start(&pemu, cemu, cemu_cmdline, cemu_path, job);
}

bool ZLauncher::launchSpeedhack(HANDLE job)
{
    return Process::start(&psh, speedhack_path, str(), Utils::getCurrentDir(), job);
}

bool ZLauncher::restartElevated(bool launched)
{
    return Process::startElevated(args[0], L" -config " + cfg + (launched ? L" -gamelaunched" : L""), Utils::getCurrentDir(), job);
}

void ZLauncher::showError()
{
    DWORD err = GetLastError();

    if(err == ERROR_SUCCESS || err == ERROR_CANCELLED || err == ERROR_INVALID_HANDLE) return;

    wcout << L"WinAPI error " << err;

    LPTSTR msg = 0;
    DWORD len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPTSTR) &msg, 0, NULL);

    if(len > 0 && msg)
    {
        wcout << L": " << msg;

        mb(msg);
        LocalFree(msg);
        msg = 0;
    }

    wcout << endl;
}

str ZLauncher::getConfigFile()
{
    return cfg;
}
