#include "ZLauncher.h"

bool Process::isElevated(HANDLE process)
{
    bool fRet = false;
    HANDLE hToken = NULL;
    if(OpenProcessToken(process, TOKEN_QUERY, &hToken))
    {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if(GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
        {
            fRet = Elevation.TokenIsElevated;
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

    if(hToken)
    {
        CloseHandle(hToken);
    }
    return fRet;
}

bool Process::start(LPPROCESS_INFORMATION inf, str file, str cmdline, str dir, HANDLE job, bool wait)
{
    STARTUPINFO sinf;
    PROCESS_INFORMATION pinf;

    ZeroMemory(&sinf, sizeof(sinf));
    sinf.cb = sizeof(sinf);
    ZeroMemory(&pinf, sizeof(pinf));

    if(CreateProcess(c(file), c(cmdline), NULL, NULL, FALSE, NULL, NULL, c(dir), &sinf, &pinf))
    {
        if(inf) *inf = pinf;
        if(job) AssignProcessToJobObject(job, pinf.hProcess);
        if(wait) WaitForSingleObject(pinf.hProcess, INFINITE);
        return true;
    }
    else
    {
        ZLauncher::showError();
    }

    return false;
}

bool Process::startElevated(str file, str cmdline, str dir, HANDLE job, bool wait)
{
    SHELLEXECUTEINFO inf = {0};
    inf.cbSize = sizeof(SHELLEXECUTEINFO);
    inf.fMask = SEE_MASK_NOCLOSEPROCESS;
    inf.hwnd = NULL;
    inf.lpVerb = L"runas";
    inf.lpFile = c(file);
    inf.lpParameters = c(cmdline);
    inf.lpDirectory = c(dir);
    inf.nShow = SW_HIDE;
    inf.hInstApp = NULL;
    if(ShellExecuteEx(&inf))
    {
        if(job) AssignProcessToJobObject(job, inf.hProcess);
        if(wait) WaitForSingleObject(inf.hProcess, INFINITE);
        return true;
    }
    else
    {
        ZLauncher::showError();
    }

    return false;
}

HANDLE Process::find(str name)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(Process32First(snapshot, &entry))
    {
        while(Process32Next(snapshot, &entry))
        {
            if(StrCmpI(entry.szExeFile, name.c_str()) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                CloseHandle(snapshot);

                return hProcess;
            }
        }
    }
    else
    {
        ZLauncher::showError();
    }

    return nullptr;
}

HANDLE Process::find(DWORD pid)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(Process32First(snapshot, &entry))
    {
        while(Process32Next(snapshot, &entry))
        {
            if(entry.th32ProcessID == pid)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                CloseHandle(snapshot);

                return hProcess;
            }
        }
    }
    else
    {
        ZLauncher::showError();
    }

    return nullptr;
}

HANDLE Process::findParent(DWORD pid)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(Process32First(snapshot, &entry))
    {
        while(Process32Next(snapshot, &entry))
        {
            if(entry.th32ProcessID == pid)
            {
                CloseHandle(snapshot);

                return Process::find(entry.th32ParentProcessID);
            }
        }
    }
    else
    {
        ZLauncher::showError();
    }

    return nullptr;
}

str Process::name(HANDLE process)
{
    TCHAR buffer[512];

    if(GetModuleFileNameEx(process, NULL, (LPTSTR) &buffer, sizeof(buffer)))

    if(*buffer == 0) return str();

    return Utils::filename(str(buffer));
}

str Process::parentName(DWORD pid)
{
    return Process::name(Process::findParent(pid));
}
