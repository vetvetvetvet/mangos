#include "../../hook.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

void hooks::cframe() {
    while (true) {
        if (!globals::firstreceived) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (globals::misc::cframe) {
            globals::misc::cframekeybind.update();

            bool should_cframe = false;
            switch (globals::misc::cframekeybind.type) {
            case keybind::TOGGLE:
                if (globals::misc::cframekeybind.enabled)
                    should_cframe = true;
                break;
            case keybind::HOLD:
                should_cframe = globals::misc::cframekeybind.enabled;
                break;
            case keybind::ALWAYS:
                should_cframe = true;
                break;
            }

            if (!should_cframe) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            // Apply cframe movement
            auto humanoid = globals::instances::lp.humanoid;
            if (is_valid_address(humanoid.address)) {
                humanoid.write_walkspeed(static_cast<float>(globals::misc::cframespeed));
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
