#include "../combat.h"
#include "../../../util/console/console.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <random>
#include <unordered_map>
#include "../../../util/driver/driver.h"
#include "../../hook.h"
#include <windows.h>
#include "../../wallcheck/wallcheck.h"

// Performance optimization: Simple frame rate limiting
static auto lastFrameTime = std::chrono::steady_clock::now();
static const auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS


#define max
#undef max
#define min
#undef min

using namespace roblox;
static bool foundTarget = false;
static auto lastFlickTime = std::chrono::steady_clock::time_point{};
// Baseline view/cursor captured when we first lock a target; used to restore on flick
static bool hasBaselineView = false;
static Matrix3 baselineCameraRotation = {};
static POINT baselineCursorPos = { 0, 0 };

struct MouseSettings {
    float baseDPI = 800.0f;
    float currentDPI = 800.0f;
    float dpiScaleFactor = 1.0f;
    bool dpiAutoDetected = false;

    void updateDPIScale() {
        dpiScaleFactor = baseDPI / currentDPI;
    }

    float getDPIAdjustedSensitivity() const {
        return dpiScaleFactor;
    }
} mouseSettings;

float CalculateDistance(Vector2 first, Vector2 sec) {
    return sqrt(pow(first.x - sec.x, 2) + pow(first.y - sec.y, 2));
}

float CalculateDistance1(Vector3 first, Vector3 sec) {
    return sqrt(pow(first.x - sec.x, 2) + pow(first.y - sec.y, 2) + pow(first.z - sec.z, 2));
}

// Helper function to check if target is jumping (in air)
bool isTargetJumping(const roblox::player& target) {
    if (!is_valid_address(target.hrp.address)) return false;
    
    Vector3 velocity = target.hrp.get_velocity();
    float vertical_velocity = velocity.y;
    
    // Consider jumping if vertical velocity is positive and significant
    return vertical_velocity > 5.0f;
}

// Helper function to get target part based on airpart settings
roblox::instance getTargetPart(const roblox::player& target, int partIndex) {
    switch (partIndex) {
        case 0: return target.head;
        case 1: return target.uppertorso;
        case 2: return target.lowertorso;
        case 3: return target.hrp;
        case 4: return target.lefthand;
        case 5: return target.righthand;
        case 6: return target.leftfoot;
        case 7: return target.rightfoot;
        case 8: {
            // Closest Part logic
            struct Candidate { roblox::instance inst; };
            std::vector<Candidate> candidates;
            candidates.push_back({ target.head });
            candidates.push_back({ target.uppertorso });
            candidates.push_back({ target.lowertorso });
            candidates.push_back({ target.hrp });
            candidates.push_back({ target.lefthand });
            candidates.push_back({ target.righthand });
            candidates.push_back({ target.leftfoot });
            candidates.push_back({ target.rightfoot });
            
            POINT p; HWND rw = FindWindowA(nullptr, "Roblox"); 
            if (rw) { 
                GetCursorPos(&p); 
                ScreenToClient(rw, &p); 
            }
            Vector2 mouse = { (float)p.x, (float)p.y };
            float bestDist = 9e18f;
            roblox::instance bestPart = target.head;
            
            for (auto& c : candidates) {
                if (!is_valid_address(c.inst.address)) continue;
                Vector2 scr = roblox::worldtoscreen(c.inst.get_pos());
                if (scr.x == -1.0f || scr.y == -1.0f) continue;
                float dx = mouse.x - scr.x; 
                float dy = mouse.y - scr.y; 
                float d2 = dx * dx + dy * dy;
                if (d2 < bestDist) { 
                    bestDist = d2; 
                    bestPart = c.inst; 
                }
            }
            return bestPart;
        }
        case 9: {
            // Random Part logic
            std::vector<roblox::instance> candidates = {
                target.head,
                target.uppertorso,
                target.lowertorso,
                target.hrp,
                target.lefthand,
                target.righthand,
                target.leftfoot,
                target.rightfoot
            };
            // Filter out invalid candidates
            std::vector<roblox::instance> validCandidates;
            for (auto& c : candidates) {
                if (is_valid_address(c.address)) {
                    validCandidates.push_back(c);
                }
            }
            if (!validCandidates.empty()) {
                int randomIndex = rand() % validCandidates.size();
                return validCandidates[randomIndex];
            } else {
                return target.head; // fallback
            }
        }
        default: 
            return target.head;
    }
}

Vector3 lerp_vector3(const Vector3& a, const Vector3& b, float t) {
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
}

Vector3 AddVector3(const Vector3& first, const Vector3& sec) {
    return { first.x + sec.x, first.y + sec.y, first.z + sec.z };
}

Vector3 DivVector3(const Vector3& first, const Vector3& sec) {
    return { first.x / sec.x, first.y / sec.y, first.z / sec.z };
}

Vector3 normalize(const Vector3& vec) {
    float length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return (length != 0) ? Vector3{ vec.x / length, vec.y / length, vec.z / length } : vec;
}

Vector3 crossProduct(const Vector3& vec1, const Vector3& vec2) {
    return {
        vec1.y * vec2.z - vec1.z * vec2.y,
        vec1.z * vec2.x - vec1.x * vec2.z,
        vec1.x * vec2.y - vec1.y * vec2.x
    };
}

Matrix3 LookAtToMatrix(const Vector3& cameraPosition, const Vector3& targetPosition) {
    Vector3 forward = normalize(Vector3{ (targetPosition.x - cameraPosition.x), (targetPosition.y - cameraPosition.y), (targetPosition.z - cameraPosition.z) });
    Vector3 right = normalize(crossProduct({ 0, 1, 0 }, forward));
    Vector3 up = crossProduct(forward, right);

    Matrix3 lookAtMatrix{};
    lookAtMatrix.data[0] = -right.x;  lookAtMatrix.data[1] = up.x;  lookAtMatrix.data[2] = -forward.x;
    lookAtMatrix.data[3] = right.y;  lookAtMatrix.data[4] = up.y;  lookAtMatrix.data[5] = -forward.y;
    lookAtMatrix.data[6] = -right.z;  lookAtMatrix.data[7] = up.z;  lookAtMatrix.data[8] = -forward.z;

    return lookAtMatrix;
}

bool detectMouseDPI() {
    HWND robloxWindow = FindWindowA(0, "Roblox");
    if (!robloxWindow) return false;

    HDC hdc = GetDC(robloxWindow);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(robloxWindow, hdc);

    HKEY hkey;
    DWORD sensitivity = 10;
    DWORD size = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Mouse", 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
        RegQueryValueEx(hkey, "MouseSensitivity", NULL, NULL, (LPBYTE)&sensitivity, &size);
        RegCloseKey(hkey);
    }

    float estimatedDPI = 800.0f * (sensitivity / 10.0f) * (dpiX / 96.0f);
    mouseSettings.currentDPI = std::max(400.0f, std::min(3200.0f, estimatedDPI));
    mouseSettings.updateDPIScale();
    mouseSettings.dpiAutoDetected = true;

    return true;
}

static float applyEasing(float t, int style) {
    t = std::clamp(t, 0.0f, 1.0f);
    switch (style) {
    case 0: // None (raw)
        return t; // factor already set externally; treat as linear
    case 1: // Linear
        return t;
    case 2: // EaseInQuad
        return t * t;
    case 3: // EaseOutQuad
        return t * (2.0f - t);
    case 4: // EaseInOutQuad
        return (t < 0.5f) ? (2.0f * t * t) : (1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f);
    case 5: // EaseInCubic
        return t * t * t;
    case 6: // EaseOutCubic
        return 1.0f - std::pow(1.0f - t, 3.0f);
    case 7: // EaseInOutCubic
        return (t < 0.5f) ? (4.0f * t * t * t) : (1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f);
    case 8: // EaseInSine
        return 1.0f - std::cos((t * 3.14159265f) / 2.0f);
    case 9: // EaseOutSine
        return std::sin((t * 3.14159265f) / 2.0f);
    case 10: // EaseInOutSine
        return -(std::cos(3.14159265f * t) - 1.0f) / 2.0f;
    default:
        return t;
    }
}

void performCameraAimbot(const Vector3& targetPos, const Vector3& cameraPos) {
    roblox::camera camera = globals::instances::camera;
    Matrix3 currentRotation = camera.getRot();
    Matrix3 targetMatrix = LookAtToMatrix(cameraPos, targetPos);

    // Apply shake if enabled
        if (globals::combat::camlock_shake) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> shakeDist(-1.0f, 1.0f);
        
        // Generate shake offsets for X and Y axes
        float shakeOffsetX = shakeDist(gen) * globals::combat::camlock_shake_x;
        float shakeOffsetY = shakeDist(gen) * globals::combat::camlock_shake_y;
        
        // Apply shake to the target matrix by slightly adjusting the rotation
        // This creates a subtle shaking effect while maintaining the aim direction
        Matrix3 shakenMatrix = targetMatrix;
        
        // Apply shake to the rotation matrix (affecting pitch and yaw)
        // Adjust the forward vector (first row) for X shake
        shakenMatrix.data[0] += shakeOffsetX * 0.01f;
        shakenMatrix.data[1] += shakeOffsetY * 0.01f;
        
        // Adjust the up vector (second row) for additional Y shake
        shakenMatrix.data[4] += shakeOffsetY * 0.01f;
        
        // Normalize the matrix to prevent distortion
        Vector3 forward = { shakenMatrix.data[0], shakenMatrix.data[1], shakenMatrix.data[2] };
        Vector3 up = { shakenMatrix.data[3], shakenMatrix.data[4], shakenMatrix.data[5] };
        Vector3 right = { shakenMatrix.data[6], shakenMatrix.data[7], shakenMatrix.data[8] };
        
        forward = normalize(forward);
        up = normalize(up);
        right = normalize(right);
        
        // Reconstruct the matrix with normalized vectors
        shakenMatrix.data[0] = forward.x; shakenMatrix.data[1] = forward.y; shakenMatrix.data[2] = forward.z;
        shakenMatrix.data[3] = up.x; shakenMatrix.data[4] = up.y; shakenMatrix.data[5] = up.z;
        shakenMatrix.data[6] = right.x; shakenMatrix.data[7] = right.y; shakenMatrix.data[8] = right.z;
        
        targetMatrix = shakenMatrix;
    }

    if (globals::combat::smoothing) {
        float baseX = std::clamp(1.0f / globals::combat::smoothingx, 0.01f, 1.0f);
        float baseY = std::clamp(1.0f / globals::combat::smoothingy, 0.01f, 1.0f);
        float easedX = applyEasing(baseX, globals::combat::smoothingstyle);
        float easedY = applyEasing(baseY, globals::combat::smoothingstyle);
        // Apply camlock sensitivity as a multiplier to the easing factor (only if sensitivity is enabled)
        // Ensure sensitivity doesn't make movement too slow (minimum 0.1f to prevent excessive smoothness)
        float sens = globals::combat::sensitivity_enabled ? std::clamp(globals::combat::cam_sensitivity, 0.1f, 5.0f) : 1.0f;
        easedX = std::clamp(easedX * sens, 0.0f, 1.0f);
        easedY = std::clamp(easedY * sens, 0.0f, 1.0f);

        Matrix3 smoothedRotation{};
        for (int i = 0; i < 9; ++i) {
            const float factor = (i % 3 == 0) ? easedX : easedY;
            smoothedRotation.data[i] = currentRotation.data[i] + (targetMatrix.data[i] - currentRotation.data[i]) * factor;
        }
        camera.writeRot(smoothedRotation);
    }
    else {
        // Without smoothing, completely ignore all sensitivity and apply instant lock
        // This ensures no partial movement that could feel like smoothing
        // Force instant lock by directly setting the target rotation
        camera.writeRot(targetMatrix);
    }
}
void performMouseAimbot(const Vector2& screenCoords, HWND robloxWindow) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(robloxWindow, &cursorPos);

    float deltaX = screenCoords.x - cursorPos.x;
    float deltaY = screenCoords.y - cursorPos.y;

    if (globals::combat::smoothing) {
        float baseX = std::clamp(1.0f / (globals::combat::smoothingx * 0.05f), 0.01f, 1.0f);
        float baseY = std::clamp(1.0f / (globals::combat::smoothingy * 0.05f), 0.01f, 1.0f);
        float easedX = applyEasing(baseX, globals::combat::smoothingstyle);
        float easedY = applyEasing(baseY, globals::combat::smoothingstyle);

        float dpiAdjustedSensitivity = mouseSettings.getDPIAdjustedSensitivity();
        // Apply DPI and user mouselock sensitivity (only if sensitivity is enabled)
        float userSens = globals::combat::sensitivity_enabled ? std::clamp(globals::combat::mouse_sensitivity, 0.01f, 5.0f) : 1.0f;
        easedX = std::clamp(easedX * dpiAdjustedSensitivity * userSens, 0.0f, 1.0f);
        easedY = std::clamp(easedY * dpiAdjustedSensitivity * userSens, 0.0f, 1.0f);

        deltaX *= easedX;
        deltaY *= easedY;
    }
    else {
        // Without smoothing, completely ignore all sensitivity and DPI adjustments for instant lock
        // deltaX and deltaY remain unchanged for truly instant movement
    }

    // Apply minimum movement threshold to prevent micro-jitters
    const float minMovementThreshold = 0.5f;
    if (std::isfinite(deltaX) && std::isfinite(deltaY) && (abs(deltaX) > minMovementThreshold || abs(deltaY) > minMovementThreshold)) {
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dx = static_cast<LONG>(deltaX);
        input.mi.dy = static_cast<LONG>(deltaY);
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(input));
    }
}

// Helper function to get target closest to camera
roblox::player gettargetclosesttocamera() {
    // Add custom models to the player list
    std::vector<roblox::player> custom_models = GetCustomModels(base_address);
    std::vector<roblox::player> all_players = globals::instances::cachedplayers;
    all_players.insert(all_players.end(), custom_models.begin(), custom_models.end());
    const auto& players = all_players;
    
    if (players.empty()) return {};

    const bool useKnockCheck = false;
    const bool useHealthCheck = false;
    const bool useFov = globals::combat::usefov;
    const float fovSize = globals::combat::fovsize;
    const float fovSizeSquared = fovSize * fovSize;
    const float healthThreshold = 0.0f;
    const bool isArsenal = (globals::instances::gamename == "Arsenal");
    const std::string& localPlayerName = globals::instances::localplayer.get_name();
    const Vector3 cameraPos = globals::instances::camera.getPos();

    roblox::player closest = {};
    float closestDistanceSquared = 9e18f;

    for (auto player : players) {
        if (!is_valid_address(player.main.address) || !is_valid_address(player.head.address)) continue;
        if (player.name == localPlayerName) continue;

        // Arsenal-specific checks
        if (isArsenal) {
            if (useHealthCheck && player.health <= healthThreshold) continue;
            if (useKnockCheck) {
                try {
                    if (player.bodyeffects.findfirstchild("K.O").read_bool_value()) continue;
                } catch (...) { continue; }
            }
        }

        // Generic checks
        if (useHealthCheck && player.health <= healthThreshold) continue;
        if (useKnockCheck) {
            try {
                if (player.bodyeffects.findfirstchild("K.O").read_bool_value()) continue;
            } catch (...) { continue; }
        }

        // Team check
        if (globals::combat::teamcheck) {
            if (player.team.address == globals::instances::lp.team.address) continue;
        }

        // Grabbed check
        if (globals::combat::grabbedcheck) {
            try {
                if (player.bodyeffects.findfirstchild("Grabbed").read_bool_value()) continue;
            } catch (...) { continue; }
        }

        Vector3 targetPos = player.hrp.get_pos();
        float distanceSquared = CalculateDistance1(cameraPos, targetPos);

        // FOV check
        if (useFov) {
            Vector2 screenPos = roblox::worldtoscreen(targetPos);
            if (screenPos.x == -1.0f || screenPos.y == -1.0f) continue;
            
            Vector2 screenCenter = { 960.0f, 540.0f }; // Assuming 1920x1080, adjust as needed
            float screenDistanceSquared = (screenPos.x - screenCenter.x) * (screenPos.x - screenCenter.x) + 
                                        (screenPos.y - screenCenter.y) * (screenPos.y - screenCenter.y);
            if (screenDistanceSquared > fovSizeSquared) continue;
        }

        // No wall check - treat all targets as visible
        if (distanceSquared < closestDistanceSquared) {
            closestDistanceSquared = distanceSquared;
            closest = player;
        }
    }

    // Return closest target
    return closest;
}

roblox::player gettargetclosesttomouse() {
    static HWND robloxWindow = nullptr;
    static auto lastWindowCheck = std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    if (!robloxWindow || std::chrono::duration_cast<std::chrono::seconds>(now - lastWindowCheck).count() > 5) {
        robloxWindow = FindWindowA(nullptr, "Roblox");
        lastWindowCheck = now;
    }

    if (!robloxWindow) return {};

    POINT point;
    if (!GetCursorPos(&point) || !ScreenToClient(robloxWindow, &point)) {
        return {};
    }

    const Vector2 curpos = { static_cast<float>(point.x), static_cast<float>(point.y) };
    
    // Add custom models to the player list
    std::vector<roblox::player> custom_models = GetCustomModels(base_address);
    std::vector<roblox::player> all_players = globals::instances::cachedplayers;
    all_players.insert(all_players.end(), custom_models.begin(), custom_models.end());
    const auto& players = all_players;

    if (players.empty()) return {};

    const bool useKnockCheck = false;
    const bool useHealthCheck = false;
    const bool useWallCheck = false;
    const bool allowWallFallback = true; // fallback built-in; UI toggle optional
            const bool useFov = globals::combat::usefov;
        const float fovSize = globals::combat::fovsize;
        const float fovSizeSquared = fovSize * fovSize;
    const float healthThreshold = 0.0f;
    const bool isArsenal = (globals::instances::gamename == "Arsenal");
    const std::string& localPlayerName = globals::instances::localplayer.get_name();
    const Vector3 cameraPos = globals::instances::camera.getPos();

    roblox::player closest = {};
    roblox::player closestVisible = {};
    float closestDistanceSquared = 9e18f;
    float closestVisibleDistanceSquared = 9e18f;

    for (auto player : players) {
        if (!is_valid_address(player.head.address) ||
            player.name == localPlayerName ||
            player.head.address == 0) {
            continue;
        }

        // Skip whitelisted players (user-chosen: aimbot should not target them)
        if (std::find(globals::instances::whitelist.begin(), globals::instances::whitelist.end(), player.name) != globals::instances::whitelist.end()) {
            continue;
        }

        // Team check - skip teammates if teamcheck is enabled
        if (globals::combat::teamcheck && globals::is_teammate(player)) {
            continue; // Skip teammate
        }

        // Grabbed check - skip players who are grabbed if enabled
        if (globals::combat::grabbedcheck && globals::is_grabbed(player)) {
            continue; // Skip grabbed player
        }

        const Vector2 screenCoords = roblox::worldtoscreen(player.head.get_pos());
        if (screenCoords.x == -1.0f || screenCoords.y == -1.0f) continue;

        const float dx = curpos.x - screenCoords.x;
        const float dy = curpos.y - screenCoords.y;
        const float distanceSquared = dx * dx + dy * dy;

        if (useFov && distanceSquared > fovSizeSquared) continue;

        if (isArsenal) {
            auto nrpbs = player.main.findfirstchild("NRPBS");
            if (nrpbs.address) {
                auto health = nrpbs.findfirstchild("Health");
                if (health.address && (health.read_double_value() == 0.0 || player.hrp.get_pos().y < 0.0f)) {
                    continue;
                }
            }
        }

        // All secondary checks removed; select purely by FOV distance
        bool isVisible = true;

        if (isVisible) {
            if (distanceSquared < closestVisibleDistanceSquared) {
                closestVisibleDistanceSquared = distanceSquared;
                closestVisible = player;
            }
        } else {
            if (distanceSquared < closestDistanceSquared) {
                closestDistanceSquared = distanceSquared;
                closest = player;
            }
        }
    }

    if (is_valid_address(closestVisible.main.address)) return closestVisible;
    if (allowWallFallback) return closest; // fallback to occluded target if none visible
    return {};
}

void hooks::aimbot() {
    if (!mouseSettings.dpiAutoDetected) {
        detectMouseDPI();
    }

    HWND robloxWindow = FindWindowA(0, "Roblox");
    roblox::player target;

    while (true) {
        auto frameStart = std::chrono::steady_clock::now();
        
        globals::combat::aimbotkeybind.update();

        if (!globals::focused || !globals::combat::aimbot || !globals::combat::aimbotkeybind.enabled) {
            foundTarget = false;
            target = {};
            // Clear aimbot lock state when aimbot is disabled
            globals::combat::aimbot_locked = false;
            globals::combat::aimbot_current_target = {};
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // Sticky aim behavior: if disabled, reacquire every tick; if enabled, keep until invalid/out-of-fov
        // If we recently detected a flick, enforce a brief cooldown before reacquiring
        if (globals::combat::arsenal_flick_fix && lastFlickTime.time_since_epoch().count() != 0) {
            auto nowCooldown = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(nowCooldown - lastFlickTime).count() < 200) {
                target = {};
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
        }

        if (!globals::combat::stickyaim || !foundTarget) {
            // Use target method to determine which function to call
            if (globals::combat::target_method == 0) {
                target = gettargetclosesttomouse();
            } else {
                target = gettargetclosesttocamera();
            }
            
            if (is_valid_address(target.main.address) && target.head.address != 0) {
                foundTarget = true;
                // Capture baseline when we first acquire a target
                hasBaselineView = true;
                baselineCameraRotation = globals::instances::camera.getRot();
                POINT tmp; if (GetCursorPos(&tmp)) baselineCursorPos = tmp;
            } else {
                foundTarget = false;
                continue;
            }
        }

        // Determine target part with airpart support
        roblox::instance targetPart;
        
        // Check if airpart is enabled and target is jumping
        if (globals::combat::airpart_enabled && isTargetJumping(target)) {
            // Use airpart when target is jumping
            targetPart = getTargetPart(target, globals::combat::airpart);
        } else {
            // Use regular aimpart when target is not jumping or airpart is disabled
            targetPart = getTargetPart(target, globals::combat::aimpart);
        }

        if (targetPart.address == 0) {
            foundTarget = false;
            continue;
        }

        Vector3 targetPos = targetPart.get_pos();

        if (globals::instances::gamename == "Arsenal" && globals::combat::unlockondeath) {
            if (target.main.findfirstchild("NRPBS").findfirstchild("Health").read_double_value() == 0 || target.hrp.get_pos().y < 0) {
                foundTarget = false;
                target = {};
                continue; // Continue to re-acquire a new target
            }
        }

        // Generic death checks (BodyEffects.Dead or Humanoid health <= 0)
        // Only unlock from dead targets if unlockondeath is enabled
        if (globals::combat::unlockondeath) {
            bool targetDead = false;
            auto bodyEffects = target.bodyeffects;
            if (bodyEffects.address) {
                auto deadFlag = bodyEffects.findfirstchild("Dead");
                if (deadFlag.address && deadFlag.read_bool_value()) {
                    targetDead = true;
                }
            }
            int targetHealth = target.humanoid.read_health();
            if (targetDead || targetHealth <= 0) {
                foundTarget = false;
                target = {};
                continue; // Continue to re-acquire a new target
            }
        }

        if (target.bodyeffects.findfirstchild("K.O").read_bool_value() && globals::combat::knockcheck && globals::combat::autoswitch && globals::combat::unlockondeath) {
            foundTarget = false;
            target = {};
            continue; // Continue to re-acquire a new target
        }

        static Vector3 lastTargetPos = { 0,0,0 };
        static Matrix3 lastGoodRotation = {};
        Vector3 predictedPos;
        if (globals::combat::predictions) {
            Vector3 velocity = targetPart.get_velocity();
            Vector3 veloVector = DivVector3(velocity, { globals::combat::predictionsx, globals::combat::predictionsy, globals::combat::predictionsx });
            predictedPos = AddVector3(targetPos, veloVector);
        } else {
            // Ensure no prediction is applied when disabled
            predictedPos = targetPos;
        }

        // Arsenal anti-flick: if target position jumps absurdly in one tick, keep previous view
        bool applyFlickGuard = (globals::instances::gamename == "Arsenal" && globals::combat::arsenal_flick_fix);
        if (applyFlickGuard && (lastTargetPos.x != 0 || lastTargetPos.y != 0 || lastTargetPos.z != 0)) {
            float jumpDistance = CalculateDistance1(predictedPos, lastTargetPos);
            if (jumpDistance > 500.0f) { // large sudden teleport threshold
                // Restore baseline view (exact original) for both modes
                if (hasBaselineView) {
                    globals::instances::camera.writeRot(baselineCameraRotation);
                    // Restore cursor position to baseline if mouse mode
                    if (globals::combat::aimbottype == 1) {
                        INPUT input = { 0 };
                        POINT cur; GetCursorPos(&cur);
                        LONG dx = baselineCursorPos.x - cur.x;
                        LONG dy = baselineCursorPos.y - cur.y;
                        input.type = INPUT_MOUSE; input.mi.dx = dx; input.mi.dy = dy; input.mi.dwFlags = MOUSEEVENTF_MOVE;
                        SendInput(1, &input, sizeof(input));
                    }
                } else if (globals::combat::aimbottype == 0 || globals::combat::aimbottype == 2) {
                    // Fallback to last good rotation if baseline missing
                    if (lastGoodRotation.data[0] != 0 || lastGoodRotation.data[4] != 0 || lastGoodRotation.data[8] != 0) {
                        globals::instances::camera.writeRot(lastGoodRotation);
                    }
                }
                // Drop lock and start cooldown so we don't follow the teleport
                foundTarget = false;
                target = {};
                lastFlickTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
                lastTargetPos = predictedPos;
                continue;
            }
        }

        Vector2 screenCoords = roblox::worldtoscreen(predictedPos);
        if (screenCoords.x == -1.0f || screenCoords.y == -1.0f) {
            if (globals::combat::stickyaim && !globals::combat::autoswitch) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            } else {
                foundTarget = false;
                continue;
            }
        }

        // Enforce FOV while locked only if sticky aim is OFF. When sticky aim is ON, keep lock even if out of FOV.
        if (globals::combat::usefov && !globals::combat::stickyaim) {
            POINT point; GetCursorPos(&point); ScreenToClient(robloxWindow, &point);
            const float dx = static_cast<float>(point.x) - screenCoords.x;
            const float dy = static_cast<float>(point.y) - screenCoords.y;
            const float distanceSquared = dx * dx + dy * dy;
            const float fovSizeSquared = globals::combat::fovsize * globals::combat::fovsize;
            if (distanceSquared > fovSizeSquared) {
                foundTarget = false;
                continue;
            }
        }

        // Additional anti-floor-flick guards
        if (applyFlickGuard) {
            // Guard 1: if the screen Y is way off the screen vertically, treat as bad frame
            math::Vector2 dims = globals::instances::visualengine.get_dimensions();
            if (screenCoords.y < -200.0f || screenCoords.y > (dims.y + 200.0f)) {
                // Restore baseline to ensure crosshair returns exactly where it was
                if (hasBaselineView) {
                    globals::instances::camera.writeRot(baselineCameraRotation);
                    if (globals::combat::aimbottype == 1) {
                        INPUT input = { 0 };
                        POINT cur; GetCursorPos(&cur);
                        LONG dx = baselineCursorPos.x - cur.x;
                        LONG dy = baselineCursorPos.y - cur.y;
                        input.type = INPUT_MOUSE; input.mi.dx = dx; input.mi.dy = dy; input.mi.dwFlags = MOUSEEVENTF_MOVE;
                        SendInput(1, &input, sizeof(input));
                    }
                } else if ((globals::combat::aimbottype == 0 || globals::combat::aimbottype == 2) && (lastGoodRotation.data[0] != 0 || lastGoodRotation.data[4] != 0 || lastGoodRotation.data[8] != 0)) {
                    globals::instances::camera.writeRot(lastGoodRotation);
                }
                foundTarget = false;
                target = {};
                lastFlickTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                lastTargetPos = predictedPos;
                continue;
            }

            // Guard 2: if the look direction points too steeply downward, keep previous rotation for this tick
            roblox::camera camTmp = globals::instances::camera;
            Matrix3 desired = LookAtToMatrix(camTmp.getPos(), predictedPos);
            // Up vector is (0,1,0); forward is -desired.data[2,5,8]
            Vector3 forward = { -desired.data[2], -desired.data[5], -desired.data[8] };
            if (forward.y < -0.9f) { // too much downward tilt
                if (hasBaselineView) {
                    globals::instances::camera.writeRot(baselineCameraRotation);
                    if (globals::combat::aimbottype == 1) {
                        INPUT input = { 0 };
                        POINT cur; GetCursorPos(&cur);
                        LONG dx = baselineCursorPos.x - cur.x;
                        LONG dy = baselineCursorPos.y - cur.y;
                        input.type = INPUT_MOUSE; input.mi.dx = dx; input.mi.dy = dy; input.mi.dwFlags = MOUSEEVENTF_MOVE;
                        SendInput(1, &input, sizeof(input));
                    }
                } else if ((globals::combat::aimbottype == 0 || globals::combat::aimbottype == 2) && (lastGoodRotation.data[0] != 0 || lastGoodRotation.data[4] != 0 || lastGoodRotation.data[8] != 0)) {
                    globals::instances::camera.writeRot(lastGoodRotation);
                }
                lastFlickTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                lastTargetPos = predictedPos;
                continue;
            }
        }

        if (globals::combat::aimbottype == 0) {
            roblox::camera camera = globals::instances::camera;
            Vector3 cameraPos = camera.getPos();
            performCameraAimbot(predictedPos, cameraPos);
            // cache last good rotation after update
            lastGoodRotation = camera.getRot();
            if (globals::combat::smoothing && !globals::combat::nosleep_aimbot) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        else if (globals::combat::aimbottype == 1) {
            performMouseAimbot(screenCoords, robloxWindow);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else if (globals::combat::aimbottype == 2) {
            // Memory Aimbot: Store target position in memory and use it for aimbot
            static Vector3 lastMemoryTargetPos = {0, 0, 0};
            static bool hasMemoryTarget = false;
            
            if (!hasMemoryTarget) {
                lastMemoryTargetPos = predictedPos;
                hasMemoryTarget = true;
            }
            
            // Use stored memory position for aiming
            roblox::camera camera = globals::instances::camera;
            Vector3 cameraPos = camera.getPos();
            performCameraAimbot(lastMemoryTargetPos, cameraPos);
            
            // Update memory position - only apply lerp when smoothing/prediction is enabled
            if (globals::combat::smoothing || globals::combat::predictions) {
                float memoryLerpSpeed = 0.1f; // Adjust this value for memory persistence
                lastMemoryTargetPos = lerp_vector3(lastMemoryTargetPos, predictedPos, memoryLerpSpeed);
            } else {
                // Direct update when no smoothing or prediction
                lastMemoryTargetPos = predictedPos;
            }
            
            // cache last good rotation after update
            lastGoodRotation = camera.getRot();
            if (globals::combat::smoothing && !globals::combat::nosleep_aimbot) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        lastTargetPos = predictedPos;

        if (target.head.address) {
            globals::instances::cachedtarget = target;
            globals::instances::cachedlasttarget = target;
            // Update global aimbot state for silent aim connection
            globals::combat::aimbot_locked = true;
            globals::combat::aimbot_current_target = target;
        } else {
            // No target, clear aimbot lock state
            globals::combat::aimbot_locked = false;
            globals::combat::aimbot_current_target = {};
        }

        // Performance optimization: Simple frame rate limiting
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        
        if (frameDuration < targetFrameTime) {
            auto sleepTime = targetFrameTime - frameDuration;
            std::this_thread::sleep_for(sleepTime);
        }
    }
}