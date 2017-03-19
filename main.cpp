#include "ZLauncher.h"

#if defined(WIN32) && defined(UNICODE)
#include <shellapi.h>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow)
{
    int argc;
    TCHAR** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#else
int main(int argc, TCHAR *argv[])
{
#endif

    FreeConsole();

    ZLauncher* launcher = new ZLauncher(argc, argv);

    return launcher->launch();
}
