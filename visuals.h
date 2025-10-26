#pragma once
#include "../../util/globals.h"
#include "../../util/classes/classes.h"
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

// Trail point data structure
struct TrailPoint {
    Vector3 position;
    std::chrono::steady_clock::time_point timestamp;
    
    TrailPoint(const Vector3& pos) : position(pos), timestamp(std::chrono::steady_clock::now()) {}
};

// Enhanced player data cache with LOD integration
struct CachedPlayerData {
    roblox::player player;
    Vector3 position;
    Vector2 top_left;
    Vector2 bottom_right;
    float distance;
    std::string name;
    std::string tool_name;
    std::string held_item_name;
    float margin;
    float health;
    bool valid;
    // PF TextLabel.TextColor3 if available
    bool hasTextColor = false;
    int textColorR = 0;
    int textColorG = 0;
    int textColorB = 0;
    // Trail data
    std::vector<TrailPoint> trail_points;
};



// Main ESP data container with thread safety
struct ESPData {
    std::vector<CachedPlayerData> cachedPlayers;

    Vector3 localPos;
    std::mutex cachedmutex;

    // Performance metrics (optional)
    std::chrono::high_resolution_clock::time_point lastUpdate;
    size_t frameCount = 0;
};

namespace visuals {
    extern ESPData espData;
    extern std::thread updateThread;
    extern bool threadRunning;
    // Core functions
    void run();
    void updateESP();
    void startThread();
    void stopThread();

    // Text rendering with FreeType support
    void draw_text_with_shadow_enhanced(float font_size, const ImVec2& position, ImColor color, const char* text);
    
    // Crosshair
    void renderCrosshair();
}

// Forward declarations for DirectX
struct ID3D11Device;
struct ID3D11DeviceContext;


namespace utils {
    bool boxes(roblox::player player);

}

void DrawChinaHat(ImDrawList* draw, const Vector3& head_pos, ImColor color);

namespace esp_helpers {
    bool on_screen(const Vector2& pos);
}
    
// Performance configuration
namespace perf_config {
    constexpr auto UPDATE_INTERVAL_HIGH = std::chrono::microseconds(2500);   // High detail updates
    constexpr auto UPDATE_INTERVAL_MEDIUM = std::chrono::microseconds(5000); // Medium detail updates  
    constexpr auto UPDATE_INTERVAL_LOW = std::chrono::microseconds(10000);   // Low detail updates
    constexpr float MAX_RENDER_DISTANCE = 500.0f;
    constexpr float MAX_OFFSCREEN_ARROW_DISTANCE = 300.0f;
    constexpr size_t MAX_CACHED_PLAYERS = 64; // Limit for performance
}