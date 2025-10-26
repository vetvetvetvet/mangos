#include "../../hook.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "../../hook.h"

static float clamp_angle(float a) {
    while (a > 180.0f) a -= 360.0f;
    while (a < -180.0f) a += 360.0f;
    return a;
}

void hooks::spinbot() {
    using clock = std::chrono::high_resolution_clock;

    static bool in_progress = false;
    static bool last_key_enabled = false;
    static float spun_deg = 0.0f; // how much we've rotated this action
    static Matrix3 startRot;      // exact starting rotation to restore
    static Vector3 startEuler;    // starting euler (degrees)

    static LARGE_INTEGER frequency;
    static LARGE_INTEGER lastTime;
    static bool timeInitialized = false;

    if (!timeInitialized) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);
        timeBeginPeriod(1);
        timeInitialized = true;
    }

    for (;;) {
        if (!globals::focused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            continue;
        }

        if (globals::misc::spin360) {
            globals::misc::spin360keybind.update();

            bool current_enabled = globals::misc::spin360keybind.enabled;
            if (current_enabled && !last_key_enabled && !in_progress) {
                in_progress = true;
                spun_deg = 0.0f;
                // Capture exact starting camera rotation
                roblox::camera camera = globals::instances::camera;
                startRot = camera.getRot();
                startEuler = startRot.MatrixToEulerAngles();
            }
            last_key_enabled = current_enabled;

            // Compute delta time
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            double dt = static_cast<double>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;
            if (dt > 0.25) dt = 0.25; // clamp

            if (in_progress) {
                // Map UI scale (1..10) to degrees per second (fast): 720..7200
                float deg_per_sec = std::clamp(globals::misc::spin360speed, 1.0f, 10.0f) * 720.0f;
                float step = deg_per_sec * static_cast<float>(dt);
                float next_total = spun_deg + step;

                roblox::camera camera = globals::instances::camera;

                if (next_total >= 360.0f) {
                    // Finish and restore exact original view
                    camera.writeRot(startRot);
                    in_progress = false;
                    spun_deg = 0.0f;
                } else {
                    Vector3 desired = startEuler;
                    desired.y = clamp_angle(startEuler.y + next_total);
                    Matrix3 newRot = startRot.EulerAnglesToMatrix(desired);
                    camera.writeRot(newRot);
                    spun_deg = next_total;
                }
            }
        } else {
            // When disabled, reset state
            in_progress = false;
            last_key_enabled = false;
            spun_deg = 0.0f;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1500));
    }
}



void hooks::hipheight() {}
