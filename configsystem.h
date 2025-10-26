#pragma once

#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "../json/json.h"
#include "../../drawing/imgui/addons/imgui_addons.h"
#include "../globals.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

class ConfigSystem {
private:
    std::string config_directory;
    // Waypoints removed
    std::vector<std::string> config_files;
    std::string current_config_name;
    char config_name_buffer[256] = "";
    char code_input_buffer[64] = "";
    char code_copy_name_buffer[256] = "";

    // Helpers
    std::string get_config_path(const std::string& name) const;
    // Resolve config path (JSON only)
    std::string resolve_existing_config_path(const std::string& name) const;
    bool is_valid_code_format(const std::string& code);
    std::string generate_config_code();
    bool find_config_by_code(const std::string& code, std::string& out_config_name);
    bool copy_config_file(const std::string& from_name, const std::string& to_name);
    bool get_config_code(const std::string& name, std::string& out_code);
    bool copy_text_to_clipboard(const std::string& text);

    // Waypoints removed

public:
    // Waypoints removed
    // Waypoint stubs used by overlay; implemented in .cpp
    std::string sanitize_filename(const std::string& name);
    bool save_waypoints_files();
    bool load_waypoints_files();

public:
    ConfigSystem();
    void refresh_config_list();
    bool save_config(const std::string& name);
    bool load_config(const std::string& name);
    bool delete_config(const std::string& name);
    void render_config_ui(float width, float height);
    const std::string& get_current_config() const;
};