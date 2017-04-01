#include "VideoPlayback.h"
#include "Utils.h"
#include "Process.h"

#include <string>
#include <fstream>
#include <iostream>

std::string vp_log_pattern = "/vol/content/Movie/";
std::string vp_video, vp_last_video;
long long vp_last_video_time;

extern HANDLE job;

extern str cemu_path, game_path;

void VideoPlayback::playVideo(std::string video)
{
    if(Utils::getTime() - vp_last_video_time < 30) return;

    HWND hCemuWnd = Utils::waitForWindow(L"wxWindowNR", NULL);
    HWND hCemuRender = Utils::waitForChildWindow(hCemuWnd, NULL, L"Render");

    str vp_window_title = L"The Legend of Zelda: Breath of the Wild cutscene player";

    PROCESS_INFORMATION vp_process;
    Process::start(&vp_process, L".\\mpv.exe", L"mpv --no-osc --keep-open=no --cursor-autohide=always --no-border --no-window-dragging --title=\"" + vp_window_title + L"\" \"" + game_path + L"\\content\\Movie\\" + Utils::s2ws(video) + L".mp4\"",
                   Utils::getCurrentDir(), job, false);

    HWND hVPWnd = Utils::waitForWindow(NULL, c(vp_window_title));

    ShowWindow(hCemuRender, SW_HIDE);
    Utils::attachWindow(hVPWnd, hCemuWnd);
    ShowWindow(hVPWnd, SW_SHOW);

    WaitForSingleObject(vp_process.hProcess, INFINITE);

    ShowWindow(hCemuRender, SW_SHOW);

    UpdateWindow(hCemuWnd);

    vp_last_video_time = Utils::getTime();
}

static void processLogLine(std::string line)
{
    size_t pos = line.find(vp_log_pattern);
    if(pos != std::string::npos)
    {
        std::string video = line.substr(pos + vp_log_pattern.length(), 9);

        VideoPlayback::playVideo(video);
    }
}

static DWORD WINAPI logWatcherThread(LPVOID arg)
{
    str filename = cemu_path + L"\\log.txt";
    #ifdef UNICODE
    std::string fn = Utils::ws2s(filename);
    #else
    std::string fn = filename;
    #endif

    std::ifstream ifs(fn, std::ios::ate);
    std::streamoff p;

    std::string line;
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
