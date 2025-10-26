#include "../../hook.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <Windows.h>

void hooks::rapidfire() {
    while (true) {
        if (!globals::firstreceived) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (globals::misc::rapidfire && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            INPUT input{};
            input.type = INPUT_MOUSE;
            input.mi.time = 0;
            input.mi.mouseData = 0;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(2, &input, sizeof(input));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
