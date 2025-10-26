#include "../../hook.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

static bool gooonnznznz = false;
void hooks::speed() {
	while (true) {
		if (!globals::firstreceived) return;
		
		globals::misc::speedkeybind.update();
		
		if (globals::misc::speed && globals::misc::speedkeybind.enabled) {
			auto humanoid = globals::instances::lp.humanoid;
			auto hrp = globals::instances::lp.hrp;
			if (!is_valid_address(humanoid.address) || !is_valid_address(hrp.address)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			
			switch (globals::misc::speedtype) {
				case 0:
					if (humanoid.read_walkspeed() != globals::misc::speedvalue) {
						humanoid.write_walkspeed(globals::misc::speedvalue);
					}
					break;
				case 1:
					auto dir = humanoid.get_move_dir();
					hrp.write_velocity(Vector3(
						dir.x * globals::misc::speedvalue,
						hrp.get_velocity().y,
						dir.z * globals::misc::speedvalue
					));
					break;
					auto dir2 = humanoid.get_move_dir();
					auto pos = hrp.get_pos();
					auto finalpos = Vector3(
						pos.x + dir2.x * globals::misc::speedvalue * 0.016f,
						pos.y,
						pos.z + dir2.z * globals::misc::speedvalue * 0.016f
					);
					hrp.write_position(finalpos);
					break;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}