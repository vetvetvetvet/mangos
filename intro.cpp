#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <Windows.h>
#include <fstream>
#include <limits>
#include <cstdio>
#include <algorithm>
#include <chrono>

#include <map>
#include <random>
#include "util/globals.h"
#include "features/wallcheck/wallcheck.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <intrin.h>
#include <ctime>
#include "util/console/console.h"
#pragma comment(lib, "urlmon.lib")

static bool injected = false;
static bool running = true;
bool robloxRunning = false;
namespace fs = std::filesystem;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
void enableANSIColors();

static bool g_authSuccess = false;
static std::string g_sessionToken = ("");
static std::chrono::steady_clock::time_point g_lastCheck;
static std::atomic<bool> g_securityThreadActive{ false };
static std::atomic<bool> g_authValidated{ false };

const std::string compilation_date = (__DATE__);
const std::string compilation_time = (__TIME__);

#pragma code_seg((".meow"))



void enableANSIColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), (("%a %m/%d/%y %H:%M:%S %Z")), &ctx);
    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10);
    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;
    localtime_s(&context, &timestamp);
    return context;
}

bool performAuthentication() {
    // auth removed - always return true
    return true;
}


__forceinline int weinit() {
    enableANSIColors();
    HWND hwnd = GetConsoleWindow();
    SetWindowText(hwnd, (("Meow")));
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 12;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
    ShowWindow(hwnd, SW_HIDE);
    tray::initialize();
    engine::startup();
    return 0;
}

#define secure __forceinline
#pragma code_seg()

int baslatici = weinit();

int main() {
    std::string weinitfakenigger = "Initializing....";
    std::string weinitamnbbibiibbibibibi = "[1] Login";
    return FLT_MAX;
}