#include "ZLauncher.h"

#include <fstream>
#include <iostream>

string vp_log_pattern = "/vol/content/Movie/";
string vp_log_pattern_ext = ".mp4";
string vp_video, vp_last_video;
long long vp_last_video_time;

extern HANDLE job;

extern str cemu_path, game_path;

static void attach(HWND child, HWND parent)
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

static long long getTimeMS()
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
static long long getTime()
{
    return getTimeMS() / 1000LL;
}

static HWND waitForWindow(LPCWSTR cls, LPCWSTR title)
{
    HWND hwnd = 0;
    while(!IsWindow(hwnd))
    {
        hwnd = FindWindow(cls, title);
        Sleep(50);
    }
    return hwnd;
}

static HWND waitForChildWindow(HWND parent, LPCWSTR cls, LPCWSTR title)
{
    HWND hwnd = 0;
    while(!IsWindow(hwnd))
    {
        hwnd = FindWindowEx(parent, NULL, cls, title);
        Sleep(50);
    }
    return hwnd;
}

void VideoPlayback::playVideo(string video)
{
    if(getTime() - vp_last_video_time < 30) return;

    HWND hCemuWnd = waitForWindow(L"wxWindowNR", NULL);
    HWND hCemuRender = waitForChildWindow(hCemuWnd, NULL, L"Render");

    str vp_window_title = L"The Legend of Zelda: Breath of the Wild cutscene player";

    PROCESS_INFORMATION vp_process;
    Process::start(&vp_process, L".\\mpv.exe", L"mpv --no-osc --keep-open=no --cursor-autohide=always --no-border --no-window-dragging --title=\"" + vp_window_title + L"\" \"" + game_path + L"\\content\\Movie\\" + Utils::s2ws(video) + L".mp4\"",
                   Utils::getCurrentDir(), job, false);

    HWND hVPWnd = waitForWindow(NULL, c(vp_window_title));

    ShowWindow(hCemuRender, SW_HIDE);
    attach(hVPWnd, hCemuWnd);
    ShowWindow(hVPWnd, SW_SHOW);

    WaitForSingleObject(vp_process.hProcess, INFINITE);

    ShowWindow(hCemuRender, SW_SHOW);

    UpdateWindow(hCemuWnd);

    vp_last_video_time = getTime();
}

static void processLogLine(string line)
{
    size_t pos = line.find(vp_log_pattern);
    if(pos != string::npos)
    {
        size_t endpos = line.find(vp_log_pattern_ext);

        string video = line.substr(pos + vp_log_pattern.length(), 9);

        VideoPlayback::playVideo(video);
    }
}

static DWORD WINAPI logWatcherThread(LPVOID arg)
{
    str filename = cemu_path + L"\\log.txt";
    #ifdef UNICODE
    string fn = Utils::ws2s(filename);
    #else
    string fn = filename;
    #endif

    ifstream ifs(fn, ios::ate);
    streamoff p;

    string line;
    while(true)
    {
        //ifs.seekg(p);

        while(getline(ifs, line))
        {
            processLogLine(line);
            p = ifs.tellg();
        }

        ifs.clear();

        Sleep(VP_LOG_CHECK_INTERVAL);
    }

    return 0;
}

void VideoPlayback::start()
{
    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, NULL, &logWatcherThread, NULL, NULL, &threadId);
}
