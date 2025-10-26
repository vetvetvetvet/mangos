#include "configsystem.h"
#include <iostream>
#include "../notification/notification.h"
#include <random>
#include <unordered_set>
#include "../classes/math/math.h"
// Waypoints removed
// Small helper to copy text to system clipboard via ImGui
bool ConfigSystem::copy_text_to_clipboard(const std::string& text) {
    if (text.empty()) return false;
    ImGui::SetClipboardText(text.c_str());
    return true;
}

// Waypoints removed
std::string ConfigSystem::sanitize_filename(const std::string& name) { return name; }

// Waypoints removed
bool ConfigSystem::save_waypoints_files() { return true; }

// Waypoints removed
bool ConfigSystem::load_waypoints_files() { return true; }

// Helper: build a full path for a config name
std::string ConfigSystem::get_config_path(const std::string& name) const {
	return config_directory + "\\" + name + ".json";
}

// Resolve an existing config path (JSON only)
std::string ConfigSystem::resolve_existing_config_path(const std::string& name) const {
	std::string json_path = config_directory + "\\" + name + ".json";
	return json_path;
}

// Helper: validate code format
bool ConfigSystem::is_valid_code_format(const std::string& code) {
    return code.length() == 14 && code.substr(0, 6) == "meow-" && 
           code.substr(6).find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos;
}

// Helper: generate a unique code with prefix "layuh-" followed by 8 uppercase letters
std::string ConfigSystem::generate_config_code() {
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(alphabet) - 2));

	while (true) {
		std::string code = "meow-";
		for (int i = 0; i < 8; ++i) code.push_back(alphabet[dist(gen)]);
		std::string dummy;
		if (!find_config_by_code(code, dummy)) return code;
	}
}

// Helper: find a config by its code (searches all files in the config directory)
bool ConfigSystem::find_config_by_code(const std::string& code, std::string& out_config_name) {
	try {
		if (!fs::exists(config_directory)) return false;
		for (const auto& entry : fs::directory_iterator(config_directory)) {
			if (!entry.is_regular_file()) continue;
			auto ext = entry.path().extension().string();
			if (ext != ".json") continue;
			std::ifstream f(entry.path(), std::ios::binary);
			if (!f.is_open()) continue;
			std::string content;
			f.seekg(0, std::ios::end);
			content.resize(static_cast<size_t>(f.tellg()));
			f.seekg(0, std::ios::beg);
			f.read(&content[0], static_cast<std::streamsize>(content.size()));
			f.close();
			json j = json::parse(content, nullptr, false);
			if (j.is_discarded()) continue;
			if (j.contains("code") && j["code"].is_string() && j["code"].get<std::string>() == code) {
				out_config_name = entry.path().stem().string();
				return true;
			}
		}
	} catch (...) {
		return false;
	}
	return false;
}

// Helper: copy a config file by name into a new name and assign a fresh unique code
bool ConfigSystem::copy_config_file(const std::string& from_name, const std::string& to_name) {
	if (from_name.empty() || to_name.empty()) return false;
	std::string from_path = resolve_existing_config_path(from_name);
	std::string to_path = get_config_path(to_name);
	if (!fs::exists(from_path)) return false;
	if (fs::exists(to_path)) return false; // don't overwrite

	std::ifstream in(from_path, std::ios::binary);
	if (!in.is_open()) return false;
	std::string content;
	in.seekg(0, std::ios::end);
	content.resize(static_cast<size_t>(in.tellg()));
	in.seekg(0, std::ios::beg);
	in.read(&content[0], static_cast<std::streamsize>(content.size()));
	in.close();

	json j = json::parse(content, nullptr, false);
	if (j.is_discarded()) return false;
	j["code"] = generate_config_code();

	std::ofstream out(to_path, std::ios::binary | std::ios::trunc);
	if (!out.is_open()) return false;
	out << j.dump(2);
	out.close();

	refresh_config_list();
	return true;
}

// Helper: read the code of a config by name
bool ConfigSystem::get_config_code(const std::string& name, std::string& out_code) {
    std::string path = resolve_existing_config_path(name);
    if (!fs::exists(path)) return false;
    try {
        std::ifstream f(path, std::ios::binary);
        if (!f.is_open()) return false;
        std::string content;
        f.seekg(0, std::ios::end);
        content.resize(static_cast<size_t>(f.tellg()));
        f.seekg(0, std::ios::beg);
        f.read(&content[0], static_cast<std::streamsize>(content.size()));
        f.close();
        json j = json::parse(content, nullptr, false);
        if (j.is_discarded()) return false;
        if (j.contains("code") && j["code"].is_string()) {
            out_code = j["code"].get<std::string>();
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

ConfigSystem::ConfigSystem() {
        char* appdata_path;
    size_t len;
    _dupenv_s(&appdata_path, &len, "APPDATA");

    if (appdata_path) {
        config_directory = std::string(appdata_path) + "\\meow\\config";
        free(appdata_path);
    }

        if (!fs::exists(config_directory)) {
        fs::create_directories(config_directory);
    }
    // Waypoints directory removed

    refresh_config_list();
}

void ConfigSystem::refresh_config_list() {
    config_files.clear();

    if (fs::exists(config_directory)) {
        std::unordered_set<std::string> seen;
        for (const auto& entry : fs::directory_iterator(config_directory)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                if (ext != ".json") continue;
                auto stem = entry.path().stem().string();
                if (seen.insert(stem).second) {
                    config_files.push_back(stem);
                }

				// Ensure every config JSON has a unique code; if missing, assign and rewrite
				try {
					std::ifstream f(entry.path(), std::ios::binary);
					if (f.is_open()) {
						std::string content;
						f.seekg(0, std::ios::end);
						content.resize(static_cast<size_t>(f.tellg()));
						f.seekg(0, std::ios::beg);
						f.read(&content[0], static_cast<std::streamsize>(content.size()));
						f.close();
						json j = json::parse(content, nullptr, false);
						if (!j.is_discarded() && (!j.contains("code") || !j["code"].is_string())) {
							j["code"] = generate_config_code();
							std::ofstream out(entry.path(), std::ios::binary | std::ios::trunc);
							if (out.is_open()) { out << j.dump(2); out.close(); }
						}
					}
				} catch (...) {
					// ignore errors ensuring code
				}
            }
        }
    }
}

bool ConfigSystem::save_config(const std::string& name) {
    if (name.empty()) return false;

    json config_json;

    // Only UI-visible combat features
    config_json["combat"]["aimbot"] = globals::combat::aimbot;
    config_json["combat"]["stickyaim"] = globals::combat::stickyaim;
    config_json["combat"]["unlockondeath"] = globals::combat::unlockondeath;
    config_json["combat"]["aimbottype"] = globals::combat::aimbottype;
    config_json["combat"]["nosleep_aimbot"] = globals::combat::nosleep_aimbot;
    config_json["combat"]["usefov"] = globals::combat::usefov;
    config_json["combat"]["drawfov"] = globals::combat::drawfov;
    config_json["combat"]["fovsize"] = globals::combat::fovsize;
    config_json["combat"]["fovcolor"] = std::vector<float>(globals::combat::fovcolor, globals::combat::fovcolor + 4);
    config_json["combat"]["smoothing"] = globals::combat::smoothing;
    config_json["combat"]["smoothingx"] = globals::combat::smoothingx;
    config_json["combat"]["smoothingy"] = globals::combat::smoothingy;
    config_json["combat"]["smoothingstyle"] = globals::combat::smoothingstyle;
    config_json["combat"]["sensitivity_enabled"] = globals::combat::sensitivity_enabled;
    config_json["combat"]["cam_sensitivity"] = globals::combat::cam_sensitivity;
        config_json["combat"]["camlock_shake"] = globals::combat::camlock_shake;
        config_json["combat"]["camlock_shake_x"] = globals::combat::camlock_shake_x;
        config_json["combat"]["camlock_shake_y"] = globals::combat::camlock_shake_y;
    config_json["combat"]["mouse_sensitivity"] = globals::combat::mouse_sensitivity;
    config_json["combat"]["predictions"] = globals::combat::predictions;
    config_json["combat"]["predictionsx"] = globals::combat::predictionsx;
    config_json["combat"]["predictionsy"] = globals::combat::predictionsy;
    config_json["combat"]["deadzone"] = globals::combat::deadzone;
    config_json["combat"]["deadzonex"] = globals::combat::deadzonex;
    config_json["combat"]["deadzoney"] = globals::combat::deadzoney;
    config_json["combat"]["teamcheck"] = globals::combat::teamcheck;
    config_json["combat"]["grabbedcheck"] = globals::combat::grabbedcheck;
    config_json["combat"]["arsenal_flick_fix"] = globals::combat::arsenal_flick_fix;
    config_json["combat"]["aimpart"] = globals::combat::aimpart;
    config_json["combat"]["airpart_enabled"] = globals::combat::airpart_enabled;
    config_json["combat"]["airpart"] = globals::combat::airpart;
    config_json["combat"]["target_method"] = globals::combat::target_method;
    config_json["combat"]["silentaim"] = globals::combat::silentaim;
    config_json["combat"]["stickyaimsilent"] = globals::combat::stickyaimsilent;
    config_json["combat"]["hitchance"] = globals::combat::hitchance;
    config_json["combat"]["closestpartsilent"] = globals::combat::closestpartsilent;
    config_json["combat"]["silentaimpart"] = globals::combat::silentaimpart;
    config_json["combat"]["usesfov"] = globals::combat::usesfov;
    config_json["combat"]["drawsfov"] = globals::combat::drawsfov;
    config_json["combat"]["sfovsize"] = globals::combat::sfovsize;
    config_json["combat"]["sfovcolor"] = std::vector<float>(globals::combat::sfovcolor, globals::combat::sfovcolor + 4);
    config_json["combat"]["silentpredictions"] = globals::combat::silentpredictions;
    config_json["combat"]["silentpredictionsx"] = globals::combat::silentpredictionsx;
    config_json["combat"]["silentpredictionsy"] = globals::combat::silentpredictionsy;
    config_json["combat"]["triggerbot"] = globals::combat::triggerbot;
    config_json["combat"]["delay"] = globals::combat::delay;


    config_json["combat"]["keybinds"]["aimbotkeybind"]["key"] = globals::combat::aimbotkeybind.key;
    config_json["combat"]["keybinds"]["aimbotkeybind"]["type"] = static_cast<int>(globals::combat::aimbotkeybind.type);
    config_json["combat"]["keybinds"]["silentaimkeybind"]["key"] = globals::combat::silentaimkeybind.key;
    config_json["combat"]["keybinds"]["silentaimkeybind"]["type"] = static_cast<int>(globals::combat::silentaimkeybind.type);
    config_json["combat"]["keybinds"]["triggerbotkeybind"]["key"] = globals::combat::triggerbotkeybind.key;
    config_json["combat"]["keybinds"]["triggerbotkeybind"]["type"] = static_cast<int>(globals::combat::triggerbotkeybind.type);

    // Only UI-visible visual features
    config_json["visuals"]["visuals"] = globals::visuals::visuals;
    config_json["visuals"]["boxes"] = globals::visuals::boxes;
    config_json["visuals"]["lockedindicator"] = globals::visuals::lockedindicator;
    config_json["visuals"]["boxtype"] = globals::visuals::boxtype;
    config_json["visuals"]["healthbar"] = globals::visuals::healthbar;
    config_json["visuals"]["healthtext"] = globals::visuals::healthtext;
    config_json["visuals"]["name"] = globals::visuals::name;
    config_json["visuals"]["nametype"] = globals::visuals::nametype;
    config_json["visuals"]["toolesp"] = globals::visuals::toolesp;
    config_json["visuals"]["helditemp"] = globals::visuals::helditemp;
    config_json["visuals"]["distance"] = globals::visuals::distance;
    config_json["visuals"]["skeletons"] = globals::visuals::skeletons;
    config_json["visuals"]["chinahat"] = globals::visuals::chinahat;
    config_json["visuals"]["chinahat_target_only"] = globals::visuals::chinahat_target_only;
    config_json["visuals"]["chinahat_color"] = std::vector<float>(globals::visuals::chinahat_color, globals::visuals::chinahat_color + 4);

            config_json["visuals"]["tracers"] = globals::visuals::tracers;
        config_json["visuals"]["tracers_glow"] = globals::visuals::tracers_glow;
        config_json["visuals"]["tracerstype"] = globals::visuals::tracerstype;
        

    config_json["visuals"]["sonar"] = globals::visuals::sonar;
    config_json["visuals"]["sonar_range"] = globals::visuals::sonar_range;
    config_json["visuals"]["sonar_thickness"] = globals::visuals::sonar_thickness;
    
    // Colors
    config_json["visuals"]["boxcolors"] = std::vector<float>(globals::visuals::boxcolors, globals::visuals::boxcolors + 4);
    config_json["visuals"]["boxfillcolor"] = std::vector<float>(globals::visuals::boxfillcolor, globals::visuals::boxfillcolor + 4);
    config_json["visuals"]["box_gradient_color1"] = std::vector<float>(globals::visuals::box_gradient_color1, globals::visuals::box_gradient_color1 + 4);
    config_json["visuals"]["box_gradient_color2"] = std::vector<float>(globals::visuals::box_gradient_color2, globals::visuals::box_gradient_color2 + 4);
    config_json["visuals"]["box_gradient"] = globals::visuals::box_gradient;
    config_json["visuals"]["box_gradient_rotation"] = globals::visuals::box_gradient_rotation;
    config_json["visuals"]["box_gradient_rotation_speed"] = globals::visuals::box_gradient_rotation_speed;
    config_json["visuals"]["namecolor"] = std::vector<float>(globals::visuals::namecolor, globals::visuals::namecolor + 4);
    config_json["visuals"]["healthbarcolor"] = std::vector<float>(globals::visuals::healthbarcolor, globals::visuals::healthbarcolor + 4);
    config_json["visuals"]["healthbarcolor1"] = std::vector<float>(globals::visuals::healthbarcolor1, globals::visuals::healthbarcolor1 + 4);
    config_json["visuals"]["distancecolor"] = std::vector<float>(globals::visuals::distancecolor, globals::visuals::distancecolor + 4);
    config_json["visuals"]["toolespcolor"] = std::vector<float>(globals::visuals::toolespcolor, globals::visuals::toolespcolor + 4);
    config_json["visuals"]["helditemcolor"] = std::vector<float>(globals::visuals::helditemcolor, globals::visuals::helditemcolor + 4);
    config_json["visuals"]["skeletonscolor"] = std::vector<float>(globals::visuals::skeletonscolor, globals::visuals::skeletonscolor + 4);

            config_json["visuals"]["tracerscolor"] = std::vector<float>(globals::visuals::tracerscolor, globals::visuals::tracerscolor + 4);


    config_json["visuals"]["sonarcolor"] = std::vector<float>(globals::visuals::sonarcolor, globals::visuals::sonarcolor + 4);
    config_json["visuals"]["sonar_dot_color"] = std::vector<float>(globals::visuals::sonar_dot_color, globals::visuals::sonar_dot_color + 4);

    // Persist overlay flags used for outline/glow/fill etc.
    try {
        if (globals::visuals::box_overlay_flags && !globals::visuals::box_overlay_flags->empty())
            config_json["visuals"]["overlay"]["box"] = *globals::visuals::box_overlay_flags;
        if (globals::visuals::skeleton_overlay_flags && !globals::visuals::skeleton_overlay_flags->empty())
            config_json["visuals"]["overlay"]["skeleton"] = *globals::visuals::skeleton_overlay_flags;
        if (globals::visuals::healthbar_overlay_flags && !globals::visuals::healthbar_overlay_flags->empty())
            config_json["visuals"]["overlay"]["healthbar"] = *globals::visuals::healthbar_overlay_flags;

    } catch (...) {
        // ignore serialization issues
    }

    // Only UI-visible misc features
    config_json["misc"]["speed"] = globals::misc::speed;
    config_json["misc"]["speedtype"] = globals::misc::speedtype;
    config_json["misc"]["speedvalue"] = globals::misc::speedvalue;
    
    // Save waypoints
    save_waypoints_files();
    config_json["misc"]["flight"] = globals::misc::flight;
    config_json["misc"]["flighttype"] = globals::misc::flighttype;
    config_json["misc"]["flightvalue"] = globals::misc::flightvalue;
    config_json["misc"]["spin360"] = globals::misc::spin360;
    config_json["misc"]["spin360speed"] = globals::misc::spin360speed;
    config_json["misc"]["autoreload"] = globals::misc::autoreload;
    config_json["misc"]["bikefly"] = globals::misc::bikefly;
    config_json["misc"]["vsync"] = globals::misc::vsync;
    config_json["misc"]["targethud"] = globals::misc::targethud;
    config_json["misc"]["override_overlay_fps"] = globals::misc::override_overlay_fps;
    config_json["misc"]["overlay_fps"] = globals::misc::overlay_fps;
    config_json["misc"]["streamproof"] = globals::misc::streamproof;
    
    // Desync visualizer settings
    config_json["misc"]["desync_visualizer"] = globals::misc::desync_visualizer;
    config_json["misc"]["desync_viz_color"] = std::vector<float>(globals::misc::desync_viz_color, globals::misc::desync_viz_color + 4);

    try {
        config_json["misc"]["keybinds_data"]["speedkeybind"]["key"] = globals::misc::speedkeybind.key;
        config_json["misc"]["keybinds_data"]["speedkeybind"]["type"] = static_cast<int>(globals::misc::speedkeybind.type);
        config_json["misc"]["keybinds_data"]["flightkeybind"]["key"] = globals::misc::flightkeybind.key;
        config_json["misc"]["keybinds_data"]["flightkeybind"]["type"] = static_cast<int>(globals::misc::flightkeybind.type);
        config_json["misc"]["keybinds_data"]["spin360keybind"]["key"] = globals::misc::spin360keybind.key;
        config_json["misc"]["keybinds_data"]["spin360keybind"]["type"] = static_cast<int>(globals::misc::spin360keybind.type);
    }
    catch (...) {
        config_json["misc"]["keybinds_data"]["speedkeybind"]["key"] = 0;
        config_json["misc"]["keybinds_data"]["speedkeybind"]["type"] = 1;
        config_json["misc"]["keybinds_data"]["flightkeybind"]["key"] = 0;
        config_json["misc"]["keybinds_data"]["flightkeybind"]["type"] = 1;
        config_json["misc"]["keybinds_data"]["spin360keybind"]["key"] = 0;
        config_json["misc"]["keybinds_data"]["spin360keybind"]["type"] = 1;
    }



    std::string filepath = get_config_path(name);

    // Preserve existing code on overwrite; otherwise assign a fresh unique code
    try {
        if (fs::exists(filepath)) {
            std::ifstream existing_file(filepath, std::ios::binary);
            if (existing_file.is_open()) {
                std::string existing_content;
                existing_file.seekg(0, std::ios::end);
                existing_content.resize(static_cast<size_t>(existing_file.tellg()));
                existing_file.seekg(0, std::ios::beg);
                existing_file.read(&existing_content[0], static_cast<std::streamsize>(existing_content.size()));
                existing_file.close();
                json existing_json = json::parse(existing_content, nullptr, false);
                if (!existing_json.is_discarded() && existing_json.contains("code") && existing_json["code"].is_string()) {
                    config_json["code"] = existing_json["code"].get<std::string>();
                } else {
                    config_json["code"] = generate_config_code();
                }
            }
        } else {
            config_json["code"] = generate_config_code();
        }
    } catch (...) {
        config_json["code"] = generate_config_code();
    }

    std::ofstream file(filepath, std::ios::binary | std::ios::trunc);

    if (file.is_open()) {
        file << config_json.dump(2);
        file.close();

        refresh_config_list();
        current_config_name = name;
        return true;
    }

    return false;
}

bool ConfigSystem::load_config(const std::string& name) {
    if (name.empty()) return false;

    std::string filepath = resolve_existing_config_path(name);
    if (!fs::exists(filepath)) {
        refresh_config_list();
        return false;
    }
    std::ifstream file(filepath, std::ios::binary);

    if (file.is_open()) {
        try {
            std::string content;
            file.seekg(0, std::ios::end);
            content.resize(static_cast<size_t>(file.tellg()));
            file.seekg(0, std::ios::beg);
            file.read(&content[0], static_cast<std::streamsize>(content.size()));
            file.close();

            json config_json = json::parse(content, nullptr, false);
            if (config_json.is_discarded()) {
                return false;
            }


                        if (config_json.contains("combat")) {
                auto& combat = config_json["combat"];
                if (combat.contains("aimbot")) globals::combat::aimbot = combat["aimbot"];
                if (combat.contains("stickyaim")) globals::combat::stickyaim = combat["stickyaim"];
                if (combat.contains("unlockondeath")) globals::combat::unlockondeath = combat["unlockondeath"];
                if (combat.contains("aimbottype")) globals::combat::aimbottype = combat["aimbottype"];
                if (combat.contains("nosleep_aimbot")) globals::combat::nosleep_aimbot = combat["nosleep_aimbot"];
                if (combat.contains("usefov")) globals::combat::usefov = combat["usefov"];
                if (combat.contains("drawfov")) globals::combat::drawfov = combat["drawfov"];
                if (combat.contains("fovsize")) globals::combat::fovsize = combat["fovsize"];
                if (combat.contains("fovcolor")) {
                    auto colors = combat["fovcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::combat::fovcolor[i] = colors[i];
                }
                
                if (combat.contains("smoothing")) globals::combat::smoothing = combat["smoothing"];
                if (combat.contains("smoothingx")) globals::combat::smoothingx = combat["smoothingx"];
                if (combat.contains("smoothingy")) globals::combat::smoothingy = combat["smoothingy"];
                if (combat.contains("smoothingstyle")) globals::combat::smoothingstyle = combat["smoothingstyle"];
                if (combat.contains("sensitivity_enabled")) globals::combat::sensitivity_enabled = combat["sensitivity_enabled"];
                if (combat.contains("cam_sensitivity")) globals::combat::cam_sensitivity = combat["cam_sensitivity"];
                if (combat.contains("camlock_shake")) globals::combat::camlock_shake = combat["camlock_shake"];
                if (combat.contains("camlock_shake_x")) globals::combat::camlock_shake_x = combat["camlock_shake_x"];
                if (combat.contains("camlock_shake_y")) globals::combat::camlock_shake_y = combat["camlock_shake_y"];
                if (combat.contains("mouse_sensitivity")) globals::combat::mouse_sensitivity = combat["mouse_sensitivity"];
                if (combat.contains("predictions")) globals::combat::predictions = combat["predictions"];
                if (combat.contains("predictionsx")) globals::combat::predictionsx = combat["predictionsx"];
                if (combat.contains("predictionsy")) globals::combat::predictionsy = combat["predictionsy"];
                if (combat.contains("deadzone")) globals::combat::deadzone = combat["deadzone"];
                if (combat.contains("deadzonex")) globals::combat::deadzonex = combat["deadzonex"];
                if (combat.contains("deadzoney")) globals::combat::deadzoney = combat["deadzoney"];
                if (combat.contains("teamcheck")) globals::combat::teamcheck = combat["teamcheck"];
                if (combat.contains("grabbedcheck")) globals::combat::grabbedcheck = combat["grabbedcheck"];
                if (combat.contains("arsenal_flick_fix")) globals::combat::arsenal_flick_fix = combat["arsenal_flick_fix"];
                if (combat.contains("aimpart")) globals::combat::aimpart = combat["aimpart"];
                if (combat.contains("airpart_enabled")) globals::combat::airpart_enabled = combat["airpart_enabled"];
                if (combat.contains("airpart")) globals::combat::airpart = combat["airpart"];
                if (combat.contains("target_method")) globals::combat::target_method = combat["target_method"];
                if (combat.contains("silentaim")) globals::combat::silentaim = combat["silentaim"];
                if (combat.contains("stickyaimsilent")) globals::combat::stickyaimsilent = combat["stickyaimsilent"];
                if (combat.contains("hitchance")) globals::combat::hitchance = combat["hitchance"];
                if (combat.contains("closestpartsilent")) globals::combat::closestpartsilent = combat["closestpartsilent"];
                if (combat.contains("silentaimpart")) globals::combat::silentaimpart = combat["silentaimpart"];
                if (combat.contains("usesfov")) globals::combat::usesfov = combat["usesfov"];
                if (combat.contains("drawsfov")) globals::combat::drawsfov = combat["drawsfov"];
                if (combat.contains("sfovsize")) globals::combat::sfovsize = combat["sfovsize"];
                if (combat.contains("sfovcolor")) {
                    auto colors = combat["sfovcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::combat::sfovcolor[i] = colors[i];
                }
                if (combat.contains("silentpredictions")) globals::combat::silentpredictions = combat["silentpredictions"];
                if (combat.contains("silentpredictionsx")) globals::combat::silentpredictionsx = combat["silentpredictionsx"];
                if (combat.contains("silentpredictionsy")) globals::combat::silentpredictionsy = combat["silentpredictionsy"];
                if (combat.contains("triggerbot")) globals::combat::triggerbot = combat["triggerbot"];
                if (combat.contains("delay")) globals::combat::delay = combat["delay"];


                                if (combat.contains("keybinds")) {
                    auto& keybinds = combat["keybinds"];
                    if (keybinds.contains("aimbotkeybind")) {
                        if (keybinds["aimbotkeybind"].contains("key"))
                            globals::combat::aimbotkeybind.key = keybinds["aimbotkeybind"]["key"];
                        if (keybinds["aimbotkeybind"].contains("type"))
                            globals::combat::aimbotkeybind.type = static_cast<keybind::c_keybind_type>(keybinds["aimbotkeybind"]["type"].get<int>());
                    }
                    if (keybinds.contains("silentaimkeybind")) {
                        if (keybinds["silentaimkeybind"].contains("key"))
                            globals::combat::silentaimkeybind.key = keybinds["silentaimkeybind"]["key"];
                        if (keybinds["silentaimkeybind"].contains("type"))
                            globals::combat::silentaimkeybind.type = static_cast<keybind::c_keybind_type>(keybinds["silentaimkeybind"]["type"].get<int>());
                    }

                    if (keybinds.contains("triggerbotkeybind")) {
                        if (keybinds["triggerbotkeybind"].contains("key"))
                            globals::combat::triggerbotkeybind.key = keybinds["triggerbotkeybind"]["key"];
                        if (keybinds["triggerbotkeybind"].contains("type"))
                            globals::combat::triggerbotkeybind.type = static_cast<keybind::c_keybind_type>(keybinds["triggerbotkeybind"]["type"].get<int>());
                    }
                }
            }

                        if (config_json.contains("visuals")) {
                auto& visuals = config_json["visuals"];
                if (visuals.contains("visuals")) globals::visuals::visuals = visuals["visuals"];
                if (visuals.contains("boxes")) globals::visuals::boxes = visuals["boxes"];
                if (visuals.contains("lockedindicator")) globals::visuals::lockedindicator = visuals["lockedindicator"];
                if (visuals.contains("boxtype")) globals::visuals::boxtype = visuals["boxtype"];
                if (visuals.contains("healthbar")) globals::visuals::healthbar = visuals["healthbar"];
                if (visuals.contains("healthtext")) globals::visuals::healthtext = visuals["healthtext"];
                if (visuals.contains("name")) globals::visuals::name = visuals["name"];
                if (visuals.contains("nametype")) globals::visuals::nametype = visuals["nametype"];
                if (visuals.contains("toolesp")) globals::visuals::toolesp = visuals["toolesp"];
                if (visuals.contains("helditemp")) globals::visuals::helditemp = visuals["helditemp"];
                if (visuals.contains("distance")) globals::visuals::distance = visuals["distance"];
                if (visuals.contains("skeletons")) globals::visuals::skeletons = visuals["skeletons"];
                if (visuals.contains("chinahat")) globals::visuals::chinahat = visuals["chinahat"];
                if (visuals.contains("chinahat_target_only")) globals::visuals::chinahat_target_only = visuals["chinahat_target_only"];
                if (visuals.contains("chinahat_color")) {
                    auto color = visuals["chinahat_color"];
                    if (color.is_array() && color.size() >= 4) {
                        globals::visuals::chinahat_color[0] = color[0];
                        globals::visuals::chinahat_color[1] = color[1];
                        globals::visuals::chinahat_color[2] = color[2];
                        globals::visuals::chinahat_color[3] = color[3];
                    }
                }

                if (visuals.contains("tracers")) globals::visuals::tracers = visuals["tracers"];
                if (visuals.contains("tracers_glow")) globals::visuals::tracers_glow = visuals["tracers_glow"];
                if (visuals.contains("tracerstype")) globals::visuals::tracerstype = visuals["tracerstype"];
                

                if (visuals.contains("sonar")) globals::visuals::sonar = visuals["sonar"];
                if (visuals.contains("sonar_range")) globals::visuals::sonar_range = visuals["sonar_range"];
                if (visuals.contains("sonar_thickness")) globals::visuals::sonar_thickness = visuals["sonar_thickness"];
        
                // Colors
                if (visuals.contains("boxcolors")) {
                    auto colors = visuals["boxcolors"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::boxcolors[i] = colors[i];
                }
                if (visuals.contains("boxfillcolor")) {
                    auto colors = visuals["boxfillcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::boxfillcolor[i] = colors[i];
                }
                if (visuals.contains("box_gradient")) {
                    globals::visuals::box_gradient = visuals["box_gradient"].get<bool>();
                }
                if (visuals.contains("box_gradient_rotation")) {
                    globals::visuals::box_gradient_rotation = visuals["box_gradient_rotation"].get<bool>();
                }
                if (visuals.contains("box_gradient_rotation_speed")) {
                    globals::visuals::box_gradient_rotation_speed = visuals["box_gradient_rotation_speed"].get<float>();
                }
                if (visuals.contains("box_gradient_color1")) {
                    auto colors = visuals["box_gradient_color1"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::box_gradient_color1[i] = colors[i];
                }
                if (visuals.contains("box_gradient_color2")) {
                    auto colors = visuals["box_gradient_color2"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::box_gradient_color2[i] = colors[i];
                }
                if (visuals.contains("namecolor")) {
                    auto colors = visuals["namecolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::namecolor[i] = colors[i];
                }
                if (visuals.contains("healthbarcolor")) {
                    auto colors = visuals["healthbarcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::healthbarcolor[i] = colors[i];
                }
                if (visuals.contains("healthbarcolor1")) {
                    auto colors = visuals["healthbarcolor1"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::healthbarcolor1[i] = colors[i];
                }
                if (visuals.contains("distancecolor")) {
                    auto colors = visuals["distancecolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::distancecolor[i] = colors[i];
                }
                if (visuals.contains("toolespcolor")) {
                    auto colors = visuals["toolespcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::toolespcolor[i] = colors[i];
                }
                if (visuals.contains("helditemcolor")) {
                    auto colors = visuals["helditemcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::helditemcolor[i] = colors[i];
                }
                if (visuals.contains("skeletonscolor")) {
                    auto colors = visuals["skeletonscolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::skeletonscolor[i] = colors[i];
                }

                if (visuals.contains("tracerscolor")) {
                    auto colors = visuals["tracerscolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::tracerscolor[i] = colors[i];
                }
                


                if (visuals.contains("sonarcolor")) {
                    auto colors = visuals["sonarcolor"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::sonarcolor[i] = colors[i];
                }
                if (visuals.contains("sonar_dot_color")) {
                    auto colors = visuals["sonar_dot_color"].get<std::vector<float>>();
                    for (int i = 0; i < 4 && i < colors.size(); i++) globals::visuals::sonar_dot_color[i] = colors[i];
                }

                

                // Restore overlay flags for outline/glow/fill etc.
                if (visuals.contains("overlay")) {
                    auto& ov = visuals["overlay"];
                    try {
                        if (ov.contains("box")) {
                            auto v = ov["box"].get<std::vector<int>>();
                            if (globals::visuals::box_overlay_flags) *globals::visuals::box_overlay_flags = v;
                        }
                        if (ov.contains("skeleton")) {
                            auto v = ov["skeleton"].get<std::vector<int>>();
                            if (globals::visuals::skeleton_overlay_flags) *globals::visuals::skeleton_overlay_flags = v;
                        }
                        if (ov.contains("healthbar")) {
                            auto v = ov["healthbar"].get<std::vector<int>>();
                            if (globals::visuals::healthbar_overlay_flags) *globals::visuals::healthbar_overlay_flags = v;
                        }

                    } catch (...) {
                        // ignore parse issues
                    }
                }
            }

                        if (config_json.contains("misc")) {
                auto& misc = config_json["misc"];
                if (misc.contains("speed")) globals::misc::speed = misc["speed"];
                if (misc.contains("speedtype")) globals::misc::speedtype = misc["speedtype"];
                if (misc.contains("speedvalue")) globals::misc::speedvalue = misc["speedvalue"];
                if (misc.contains("flight")) globals::misc::flight = misc["flight"];
                if (misc.contains("flighttype")) globals::misc::flighttype = misc["flighttype"];
                if (misc.contains("flightvalue")) globals::misc::flightvalue = misc["flightvalue"];
                if (misc.contains("spin360")) globals::misc::spin360 = misc["spin360"];
                if (misc.contains("spin360speed")) globals::misc::spin360speed = misc["spin360speed"];
                if (misc.contains("autoreload")) globals::misc::autoreload = misc["autoreload"];
                if (misc.contains("bikefly")) globals::misc::bikefly = misc["bikefly"];
                if (misc.contains("vsync")) globals::misc::vsync = misc["vsync"];
                if (misc.contains("targethud")) globals::misc::targethud = misc["targethud"];
                if (misc.contains("override_overlay_fps")) globals::misc::override_overlay_fps = misc["override_overlay_fps"];
                if (misc.contains("overlay_fps")) globals::misc::overlay_fps = misc["overlay_fps"];
                if (misc.contains("streamproof")) globals::misc::streamproof = misc["streamproof"];
                
                // Load desync visualizer settings
                if (misc.contains("desync_visualizer")) globals::misc::desync_visualizer = misc["desync_visualizer"];
                if (misc.contains("desync_viz_color")) {
                    auto color_array = misc["desync_viz_color"];
                    if (color_array.is_array() && color_array.size() >= 4) {
                        globals::misc::desync_viz_color[0] = color_array[0];
                        globals::misc::desync_viz_color[1] = color_array[1];
                        globals::misc::desync_viz_color[2] = color_array[2];
                        globals::misc::desync_viz_color[3] = color_array[3];
                    }
                }
                
                // Load waypoints
                // Load waypoints from dedicated folder
                load_waypoints_files();

                                if (misc.contains("keybinds_data")) {
                    auto& keybinds = misc["keybinds_data"];
                    if (keybinds.contains("speedkeybind")) {
                        if (keybinds["speedkeybind"].contains("key"))
                            globals::misc::speedkeybind.key = keybinds["speedkeybind"]["key"];
                        if (keybinds["speedkeybind"].contains("type"))
                            globals::misc::speedkeybind.type = static_cast<keybind::c_keybind_type>(keybinds["speedkeybind"]["type"].get<int>());
                    }
                    if (keybinds.contains("flightkeybind")) {
                        if (keybinds["flightkeybind"].contains("key"))
                            globals::misc::flightkeybind.key = keybinds["flightkeybind"]["key"];
                        if (keybinds["flightkeybind"].contains("type"))
                            globals::misc::flightkeybind.type = static_cast<keybind::c_keybind_type>(keybinds["flightkeybind"]["type"].get<int>());
                    }
                    if (keybinds.contains("spin360keybind")) {
                        if (keybinds["spin360keybind"].contains("key"))
                            globals::misc::spin360keybind.key = keybinds["spin360keybind"]["key"];
                        if (keybinds["spin360keybind"].contains("type"))
                            globals::misc::spin360keybind.type = static_cast<keybind::c_keybind_type>(keybinds["spin360keybind"]["type"].get<int>());
                    }
                }
            }



            current_config_name = name;
            return true;
        }
        catch (const std::exception& e) {
            return false;
        } catch (...) {
            return false;
        }
    }

    return false;
}

bool ConfigSystem::delete_config(const std::string& name) {
    if (name.empty()) return false;

    std::string filepath = resolve_existing_config_path(name);

    if (fs::exists(filepath)) {
        fs::remove(filepath);
        refresh_config_list();

        if (current_config_name == name) {
            current_config_name.clear();
        }

        return true;
    }

    return false;
}

void ConfigSystem::render_config_ui(float width, float height) {
    ImAdd::BeginChild("CONFIG SYSTEM", ImVec2(width, height));
    {
        // Single-page Manage UI only: Save, Load, Delete, List
        {
                ImGui::Text("Config Name:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##config_name", config_name_buffer, sizeof(config_name_buffer));
                ImGui::Spacing();

                ImGui::Columns(3, nullptr, false);

                if (ImAdd::Button("Save Config", ImVec2(-1, 25))) {
                    if (strlen(config_name_buffer) > 0) {
                        if (save_config(std::string(config_name_buffer))) {
                            Notifications::Success("Config '" + std::string(config_name_buffer) + "' saved successfully!");
                            memset(config_name_buffer, 0, sizeof(config_name_buffer));
                        }
                        else {
                            Notifications::Error("Failed to save config '" + std::string(config_name_buffer) + "'!");
                        }
                    }
                    else {
                        Notifications::Warning("Please enter a config name!");
                    }
                }

                ImGui::NextColumn();

                if (ImAdd::Button("Load Config", ImVec2(-1, 25))) {
                    if (strlen(config_name_buffer) > 0) {
                        if (load_config(std::string(config_name_buffer))) {
                            Notifications::Success("Config '" + std::string(config_name_buffer) + "' loaded successfully!");
                        }
                        else {
                            Notifications::Error("Failed to load config '" + std::string(config_name_buffer) + "'!");
                        }
                    }
                    else {
                        Notifications::Warning("Please enter a config name or select from list!");
                    }
                }

                ImGui::NextColumn();

                if (ImAdd::Button("Delete Config", ImVec2(-1, 25))) {
                    if (strlen(config_name_buffer) > 0) {
                        std::string config_name = std::string(config_name_buffer);
                        if (delete_config(config_name)) {
                            Notifications::Success("Config was deleted!");
                            memset(config_name_buffer, 0, sizeof(config_name_buffer));
                        }
                        else {
                            Notifications::Error("Failed To Delete!");
                        }
                    }
                    else {
                        Notifications::Warning("Select A Config!");
                    }
                }

                ImGui::Columns(1);
                ImGui::Spacing();

                if (!config_files.empty()) {
                    ImGui::Text("Available Configs:");
                    ImGui::BeginChild("config_list", ImVec2(-1, -30));
                    {
                        for (const auto& config : config_files) {
                            bool is_selected = (current_config_name == config);

                            if (ImGui::Selectable(config.c_str(), is_selected)) {
                                strcpy_s(config_name_buffer, sizeof(config_name_buffer), config.c_str());
                                Notifications::Info("Selected config: " + config, 2.0f);
                            }

                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }

                            // Show code next to config (no copy button in Manage tab)
                            std::string cfg_code;
                            if (get_config_code(config, cfg_code)) {
                                ImGui::SameLine();
                                ImGui::TextDisabled("%s", cfg_code.c_str());
                            }
                        }
                    }
                    ImGui::EndChild();

                    if (ImAdd::Button("Refresh List", ImVec2(-1, 25))) {
                        size_t old_count = config_files.size();
                        refresh_config_list();
                        size_t new_count = config_files.size();

                        if (new_count != old_count) {
                            Notifications::Info("Config list refreshed - Found " + std::to_string(new_count) + " configs", 2.0f);
                        }
                        else {
                            Notifications::Info("Config list refreshed", 1.5f);
                        }
                    }
                }
                else {

                    ImGui::Spacing();
                }

                // end Manage section
        }
    }
    ImAdd::EndChild();
}

const std::string& ConfigSystem::get_current_config() const {
    return current_config_name;
}