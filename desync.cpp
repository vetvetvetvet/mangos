#include "../../../util/globals.h"
#include "../../../util/classes/classes.h"
#include "../../../util/driver/driver.h"
#include <chrono>
#include <thread>

namespace desync
{
    void Desync()
    {
        static bool bWasActive = false;
        static int32_t iOriginalBandwidth = -1;
        static auto tRestorationStart = std::chrono::steady_clock::now();
        static bool bIsRestoring = false;
        static bool bInitialized = false;

        while (true)
        {
            if (!globals::instances::localplayer.address)
            {
                if (bInitialized && iOriginalBandwidth != -1)
                {
                    write<int32_t>(base_address + 0x62191C4, iOriginalBandwidth);
                    iOriginalBandwidth = -1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            globals::misc::desynckeybind.update();

            bool bCurrentlyActive = globals::misc::desync && globals::misc::desynckeybind.enabled;

            if (bCurrentlyActive && !bWasActive)
            {
                if (iOriginalBandwidth == -1)
                {
                    iOriginalBandwidth = read<int32_t>(base_address + 0x60EB1F4);
                    bInitialized = true;
                }
                write<int32_t>(base_address + 0x62191C4, 0);
                bIsRestoring = false;
                globals::misc::desync_active = true;

                if (globals::misc::desync_visualizer && !globals::misc::desync_ghost_created)
                {
                    if (globals::instances::lp.hrp.address != 0) {
                        globals::misc::desync_ghost_position = globals::instances::lp.hrp.get_pos();
                        globals::misc::desync_activation_pos = globals::instances::lp.hrp.get_pos();
                    }
                    else {
                        globals::misc::desync_ghost_position = globals::instances::localplayer.get_pos();
                        globals::misc::desync_activation_pos = globals::instances::localplayer.get_pos();
                    }
                    globals::misc::desync_ghost_created = true;
                }
            }
            else if (!bCurrentlyActive && bWasActive)
            {
                if (iOriginalBandwidth != -1)
                {
                    tRestorationStart = std::chrono::steady_clock::now();
                    bIsRestoring = true;
                }
                globals::misc::desync_active = false;

                if (globals::misc::desync_ghost_created)
                {
                    globals::misc::desync_ghost_created = false;
                }
            }
            else if (bCurrentlyActive)
            {
                write<int32_t>(base_address + 0x62191C4, 0);
            }

            if (bIsRestoring && iOriginalBandwidth != -1)
            {
                auto tCurrentTime = std::chrono::steady_clock::now();
                auto tElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tCurrentTime - tRestorationStart);

                if (tElapsed.count() < 3000)
                {
                    write<int32_t>(base_address + 0x62191C4, iOriginalBandwidth);
                }
                else
                {
                    bIsRestoring = false;
                    iOriginalBandwidth = -1;
                }
            }

            bWasActive = bCurrentlyActive;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}