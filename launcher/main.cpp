// Red Alert 3 (iOS) PC launcher.
// Starts the bundled touchHLE runtime with the app in a desktop-friendly way.
#include <windows.h>
#include <string>
#include <vector>

static std::wstring ExeDir() {
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    std::wstring p(buf, n);
    size_t pos = p.find_last_of(L"\\/");
    return pos == std::wstring::npos ? L"." : p.substr(0, pos);
}

static bool FileExists(const std::wstring& p) {
    DWORD a = GetFileAttributesW(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

static int ReadIniInt(const std::wstring& ini, const wchar_t* section,
                      const wchar_t* key, int def) {
    return (int)GetPrivateProfileIntW(section, key, def, ini.c_str());
}

static std::wstring Quote(const std::wstring& s) {
    return L"\"" + s + L"\"";
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    std::wstring dir = ExeDir();
    std::wstring ini = dir + L"\\ra3pc.ini";
    std::wstring runtime = dir + L"\\touchHLE.exe";
    std::wstring app = dir + L"\\touchHLE_apps\\Red Alert.app";

    if (!FileExists(runtime)) {
        MessageBoxW(nullptr, L"touchHLE.exe not found next to the launcher.",
                    L"Red Alert 3 PC", MB_ICONERROR);
        return 1;
    }
    if (!FileExists(app + L"\\Info.plist")) {
        MessageBoxW(nullptr, L"Game data not found: touchHLE_apps\\Red Alert.app",
                    L"Red Alert 3 PC", MB_ICONERROR);
        return 1;
    }

    int fullscreen = ReadIniInt(ini, L"video", L"fullscreen", 0);
    int scale = ReadIniInt(ini, L"video", L"scale", 3);
    if (scale < 1) scale = 1;
    if (scale > 4) scale = 4;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (!_wcsicmp(argv[i], L"--windowed")) fullscreen = 0;
            else if (!_wcsicmp(argv[i], L"--fullscreen")) fullscreen = 1;
        }
        LocalFree(argv);
    }

    std::wstring cmd = Quote(runtime) + L" " + Quote(app) +
                       L" --landscape-left --scale-hack=" + std::to_wstring(scale);
    if (fullscreen) cmd += L" --fullscreen";

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> mutableCmd(cmd.begin(), cmd.end());
    mutableCmd.push_back(0);

    BOOL ok = CreateProcessW(runtime.c_str(), mutableCmd.data(), nullptr, nullptr,
                             FALSE, CREATE_NO_WINDOW, nullptr, dir.c_str(), &si, &pi);
    if (!ok) {
        wchar_t msg[512];
        wsprintfW(msg, L"Failed to start the runtime (error %lu).", GetLastError());
        MessageBoxW(nullptr, msg, L"Red Alert 3 PC", MB_ICONERROR);
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
