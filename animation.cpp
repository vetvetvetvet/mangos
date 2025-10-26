#include "../../hook.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

void hooks::animationchanger() {
    while (true) {
        if (!globals::firstreceived) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            continue;
        }

        if (globals::misc::animation) {
            auto localplayer = globals::instances::lp.main;
            if (!is_valid_address(localplayer.address)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                continue;
            }

            auto animate = localplayer.findfirstchild("Animate");
            if (!is_valid_address(animate.address)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                continue;
            }

            auto fall = animate.findfirstchild("fall").findfirstchild("FallAnim");
            auto jump = animate.findfirstchild("jump").findfirstchild("jump");
            auto run = animate.findfirstchild("run").findfirstchild("RunAnim");
            auto walk = animate.findfirstchild("walk").findfirstchild("WalkAnim");
            auto idle1 = animate.findfirstchild("idle").findfirstchild("Animation1");
            auto idle2 = animate.findfirstchild("idle").findfirstchild("Animation2");

            switch (globals::misc::animationtype) {
            case 0: // vampire
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921320299");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921326949");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921322186");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921321317");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921315373");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921316709");
                break;

            case 1: // elderly
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921104374");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921111375");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921107367");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921105765");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921101664");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921102574");
                break;

            case 2: // zombie
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=616163682");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921355261");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921351278");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921350320");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921344533");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921345304");
                break;

            case 3: // stylish
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921276116");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921283326");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921279832");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921278648");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921272275");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921273958");
                break;

            case 4: // mage
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921148209");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921152678");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921149743");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921148939");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921144709");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921145797");
                break;

            case 5: // pirate
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=750783738");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=750785693");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=750782230");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=750780242");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=750781874");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=750782770");
                break;

            case 6: // cartoony
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921076136");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921082452");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921078135");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921077030");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921071918");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921072875");
                break;

            case 7: // werewolf
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921411889");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921416483");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921414272");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921413408");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921410106");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921410905");
                break;

            case 8: // robot
                if (is_valid_address(run.address)) run.write_animationid("http://www.roblox.com/asset/?id=10921250460");
                if (is_valid_address(walk.address)) walk.write_animationid("http://www.roblox.com/asset/?id=10921255446");
                if (is_valid_address(jump.address)) jump.write_animationid("http://www.roblox.com/asset/?id=10921252123");
                if (is_valid_address(fall.address)) fall.write_animationid("http://www.roblox.com/asset/?id=10921251156");
                if (is_valid_address(idle1.address)) idle1.write_animationid("http://www.roblox.com/asset/?id=10921248039");
                if (is_valid_address(idle2.address)) idle2.write_animationid("http://www.roblox.com/asset/?id=10921248831");
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
}
