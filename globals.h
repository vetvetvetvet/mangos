#pragma once
#include "../util/classes/classes.h"
#include "../drawing/overlay/keybind/keybind.h"
#include <unordered_map>
namespace globals
{
	inline bool focused;
	inline bool firstreceived = false;
	inline bool unattach = false;
	inline bool handlingtp = false;
	inline std::vector<roblox::wall> walls;
	namespace bools {
		inline bool autokill, kill;
		inline std::string name;
		inline roblox::player entity;

	}

	// Render-loop guard to avoid memory reads while drawing
	inline bool in_render_loop = false;

	// Cached teammate decisions to avoid reads during rendering
	inline std::unordered_map<uint64_t, bool> teammate_cache;
	// Debounce state: require consecutive identical verdicts before committing
	inline std::unordered_map<uint64_t, bool> teammate_last_verdict;
	inline std::unordered_map<uint64_t, int> teammate_verdict_streak;
	namespace instances {
		inline std::vector<std::string> whitelist;
		inline std::vector<std::string> blacklist;
		inline roblox::instance visualengine;
		inline roblox::instance datamodel;
		inline roblox::instance workspace;
		inline roblox::instance players;
		inline roblox::player lp;
		inline roblox::instance lighting;
		inline roblox::camera camera;
		inline roblox::instance localplayer;
		inline std::string gamename = "Universal";
        inline std::string username = "layuh";
		inline std::vector<roblox::player> cachedplayers;
		inline roblox::player cachedtarget;
		inline roblox::player cachedlasttarget;
		inline roblox::instance aim;
		inline uintptr_t mouseservice;
		inline ImDrawList* draw;
		// esp_font removed
	}
	namespace combat {
		inline bool rapidfire;
		inline bool autostomp;
				inline bool aimbot = false;
		inline bool stickyaim = true;
		inline bool unlockondeath = false; // camlock: drop lock when target dies
		inline int aimbottype = 0;
		inline keybind aimbotkeybind("aimbotkeybind");
				inline bool usefov = false;
		inline bool drawfov = false;
		inline float fovsize = 50;
		inline float fovcolor[4] = {1,1,1,1};
				inline bool smoothing = false;
		inline float smoothingx = 5;
		inline float smoothingy = 5;
		inline int smoothingstyle = 1; // 0=None(raw), 1=Linear, 2..10 easing styles
		// Sensitivity controls
		inline bool sensitivity_enabled = true; // master toggle for sensitivity
		inline float cam_sensitivity = 1.0f; // camlock sensitivity multiplier
		inline bool camlock_shake = false; // enable shake for camlock
		inline float camlock_shake_x = 2.0f; // X-axis shake intensity
		inline float camlock_shake_y = 2.0f; // Y-axis shake intensity
		inline float mouse_sensitivity = 1.0f; // mouselock sensitivity multiplier
				inline bool predictions = false;
		inline float predictionsx = 5;
		inline float predictionsy = 5;
		inline bool deadzone = false;
		inline float deadzonex = 0.0f;
		inline float deadzoney = 0.0f;
				inline bool autoswitch = false;
		inline bool teamcheck = false;
		inline bool grabbedcheck = false;
		inline bool knockcheck = false;
		inline bool rangecheck = false;
		inline bool healthcheck = false;
		inline bool wallcheck = false;
		inline bool arsenal_flick_fix = false;
		inline std::vector<int>* flags = new std::vector<int>{ 0, 0, 0, 0, 0,0,0};
		inline float range = 1000;
		inline float healththreshhold = 10;
						inline int aimpart = 0; // 0 = Head, 1 = Upper Torso, 2 = Lower Torso, 3 = HumanoidRootPart, 4 = Left Hand, 5 = Right Hand, 6 = Left Foot, 7 = Right Foot, 8 = Closest Part, 9 = Random Part
		inline bool silentaim = false;
		inline bool stickyaimsilent = true;
		inline bool connect_to_aimbot = false; // Connect silent aim to aimbot target
		inline bool aimbot_locked = false; // Track if aimbot is currently locked onto a target
		inline roblox::player aimbot_current_target; // Current target that aimbot is locked onto
		inline int hitchance = 100;
		inline keybind silentaimkeybind("silentaimkeybind");
		inline int closestpartsilent = 0; // 0 = Off, 1 = Closest Part, 2 = Closest Point
		inline int silentaimpart = 0; // 0 = Head, 1 = Upper Torso, 2 = Lower Torso, 3 = HumanoidRootPart, 4 = Left Hand, 5 = Right Hand, 6 = Left Foot, 7 = Right Foot, 8 = Closest Part, 9 = Random Part
		inline int mouselockpart = 0; // 0 = Head, 1 = Upper Torso, 2 = Lower Torso, 3 = HumanoidRootPart, 4 = Left Hand, 5 = Right Hand, 6 = Left Foot, 7 = Right Foot, 8 = Closest Part, 9 = Random Part
		inline bool nosleep_aimbot = false; // No sleep aimbot for memory and camera types only
		inline int camlockpart = 0; // 0 = Head, 1 = Upper Torso, 2 = Lower Torso, 3 = HumanoidRootPart, 4 = Left Hand, 5 = Right Hand, 6 = Left Foot, 7 = Right Foot, 8 = Closest Part, 9 = Random Part
		inline bool airpart_enabled = false; // Enable airpart feature
		inline int airpart = 0; // 0 = Head, 1 = Upper Torso, 2 = Lower Torso, 3 = HumanoidRootPart, 4 = Left Hand, 5 = Right Hand, 6 = Left Foot, 7 = Right Foot, 8 = Closest Part, 9 = Random Part
		inline int target_method = 0; // 0 = Closest To Mouse, 1 = Closest To Camera
		inline bool usesfov = false;
		inline bool drawsfov = false;
		inline float sfovsize = 50;
		inline float sfovcolor[4] = { 1,1,1,1 };
		inline bool silentpredictions = false;
		inline float silentpredictionsx = 5;
		inline float silentpredictionsy = 5;
				inline bool orbit = false;
				inline std::vector<int> orbittype = std::vector<int>(3, 0);
		inline float orbitspeed = 8;
		inline float orbitrange = 3;
		inline float orbitheight = 1;
		inline bool drawradiusring = false;
		inline keybind orbitkeybind("orbitkeybind                 ");
	//	inline std::vector<int>* orbittype;
                
				inline bool triggerbot = false;
				inline bool antiaim = false;
				inline bool underground_antiaim = false;
		inline float delay = 0;
		inline keybind triggerbotkeybind("triggerbotkeybind");
		


	}
	namespace visuals {
				inline bool visuals = true;
		inline bool boxes = false;
		inline bool lockedindicator = false;

		inline bool glowesp = false;
		inline int boxtype = 0;
		inline bool health = false;
		inline bool healthbar = false;
		inline bool armorbar = false;
		inline bool healthtext = false;
		inline bool name = false;
		inline int nametype = 0;
		inline bool toolesp = false;
		inline bool helditemp = false;
		inline bool distance = false;
		inline bool flags = false;
		inline bool skeletons = false;

		inline bool selfesp = false;
		inline bool maxdistance_enabled = false;
		inline float maxdistance = 1000.0f;
		inline bool chinahat = false;
		inline bool chinahat_target_only = false;
		inline float chinahat_color[4] = {1.0f, 0.843f, 0.0f, 1.0f};
		inline bool localplayer = false;
		

		
		inline bool predictionsdot = false;

		// Radar
		inline bool radar = false;
		inline float radar_size = 160.0f; // square side in pixels
		inline float radar_range = 250.0f; // meters
		inline float radar_pos[2] = { 20.0f, 20.0f }; // top-left corner
		inline bool radar_rotate = true;
		inline bool radar_show_distance = true;
		inline bool radar_outline = true;
		inline float radar_background[4] = { 0.06f, 0.06f, 0.06f, 0.6f };
		inline float radar_border[4] = { 0.0f, 0.0f, 0.0f, 0.8f };
		inline float radar_dot[4] = { 1.0f, 0.3f, 0.3f, 1.0f };

        inline float boxcolors[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
		        inline float boxfillcolor[4] = { 0.2,0.2,0.2,0.3 };
        inline float box_gradient_color1[4] = { 0.8f, 0.2f, 0.8f, 0.5f }; // First gradient color
        inline float box_gradient_color2[4] = { 0.2f, 0.8f, 0.8f, 0.5f }; // Second gradient color
        inline float glowcolor[4] =  { 1.0f, 0.4f, 0.4f, 1.0f};
        inline bool box_gradient = false;
        inline bool box_gradient_rotation = false; // Rotating gradient direction
        inline float box_gradient_rotation_speed = 0.25f; // revolutions per second
		inline float lockedcolor[4] =  { 1,0,0,1 };
		inline float healthbarcolor[4] = { 1,0,0,1 };
		inline float healthbarcolor1[4] = { 0,1,0,1 };
        inline float namecolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
        inline float toolespcolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
        inline float helditemcolor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                inline float distancecolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
        inline float sonarcolor[4] = { 1.0f, 1.0f, 1.0f, 0.8f };
        inline float sonar_dot_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

        inline float skeletonscolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
        inline bool sonar = false;
        inline float sonar_range = 50.0f;
        inline float sonar_thickness = 1.0f;
        inline bool sonar_show_distance = false;
        
        // Team display
        inline bool teams = false;
        inline float teamscolor[3] = { 0.3647f, 0.4471f, 0.4549f };
        inline std::vector<int>* team_overlay_flags = new std::vector<int>{ 0, 0 };
        
        // Fog controls
        inline bool fog = false;
        inline float fog_start = 0.0f;
        inline float fog_end = 1000.0f;
        inline float fog_color[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // Default gray fog color
    


				inline bool fortniteindicator = false;
		inline bool hittracer = false;
		inline bool trail = false;
		inline float trail_duration = 3.0f; // Trail duration in seconds
		inline float trail_color[4] = { 1.0f, 0.4f, 0.4f, 0.8f }; // Trail color (RGBA)
		inline float trail_thickness = 2.0f; // Trail line thickness
		inline bool hitbubble = false;
		inline bool targetskeleton = false;


		// Tracers
		inline bool tracers = false;
		inline int tracerstype = 0; // 0 Off, 1 Normal, 2 Spiderweb
		inline bool tracers_outline = false;
		inline bool tracers_glow = false;
		inline float tracerscolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
		

		inline float flagscolor[4] = { 1.0f, 0.4f, 0.4f, 1.0f };
		inline float flags_font_size = 11.0f;
				inline float flags_offset_x = 5.0f;

		// ESP distance limit removed
		inline bool enemycheck = true;
		inline bool friendlycheck = true;
		inline bool teamcheck = false;
		inline bool rangecheck = false;
		inline float range = 1000;

		
		// Crosshair
		inline bool crosshair_enabled = false;
		inline float crosshair_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		inline float crosshair_size = 10.0f;
		inline float crosshair_gap = 5.0f;
		inline bool crosshair_gapTween = false;
		inline float crosshair_gapSpeed = 1.0f;
		inline float crosshair_thickness = 1.0f;
		inline int crosshair_styleIdx = 0;
		inline float crosshair_baseSpeed = 100.0f;
		inline float crosshair_fadeDuration = 1.0f;
			inline std::vector<int>* box_overlay_flags = new std::vector<int>{ 0, 0, 0 };
		inline std::vector<int>* name_overlay_flags = new std::vector<int>{ 0, 0 };
		inline std::vector<int>* flag_overlay_flags = new std::vector<int>{ 0, 0 };
		inline std::vector<int>* healthbar_overlay_flags = new std::vector<int>{ 0, 0, 0 };
		inline std::vector<int>* tool_overlay_flags = new std::vector<int>{ 0, 0 };
		inline std::vector<int>* distance_overlay_flags = new std::vector<int>{ 0, 0 };
		inline std::vector<int>* skeleton_overlay_flags = new std::vector<int>{ 0, 0 };

		
		// Sensor
		




	}
	namespace misc {
				inline bool speed = false;
		inline int speedtype = 0;
		inline float speedvalue = 16;
		inline keybind speedkeybind("speedkeybind");
		
		// Custom Models feature
		inline bool custom_model_enabled = false;
		inline std::string custom_model_folder_path = "";
		inline std::mutex custom_model_mutex;
		// 360 Spin bind
		inline bool spin360 = false;
		// UI scale 1..10; mapped internally to a high deg/s for fast spins
		inline float spin360speed = 5.0f; // UI scale (1..10)
		inline keybind spin360keybind("spin360keybind");
				inline bool jumppower = false;
		inline float jumpowervalue = 50;
		inline keybind jumppowerkeybind("jumppowerkeybind");
                inline bool voidhide = false; // hidden from UI
        inline keybind voidhidebind("voidehidkeybind"); // kept for compat
		inline bool flight = false;
		inline int flighttype = 0;
		inline float flightvalue = 16;
		inline keybind flightkeybind("flightkeybind");
		inline bool hipheight = false;
		inline  float hipheightvalue = 16;
		inline  bool rapidfire = false;
		inline bool autoarmor = false;
		inline bool autoreload = false;
                inline bool autostomp = false; // hidden from UI
		inline bool antistomp = false;
		inline bool bikefly = false;
		inline keybind stompkeybind("stompkeybind");

		// Animation system
		inline bool animation = false;
		inline int animationtype = 0; // 0=vampire, 1=elderly, 2=zombie, 3=stylish, 4=mage, 5=pirate, 6=cartoony, 7=werewolf, 8=robot
		
		// Enhanced movement
		inline bool cframe = false;
		inline int cframespeed = 16;
		inline keybind cframekeybind("cframekeybind");
		
		// Anti-aim
		inline bool antiaim = false;

		// Desync functionality
		// Desync
		inline bool desync = false;
		inline bool desync_visualizer = false;
		inline keybind desynckeybind("desynckeybind");
		inline bool desync_active = false;
		inline roblox::instance desync_ghost_player;
		inline bool desync_ghost_created = false;
		inline Vector3 desync_ghost_position;
		
		// Desync visualizer (restored from old implementation)
		inline float desync_viz_color[4] = { 1.0f, 0.0f, 0.0f, 0.8f }; // Red with transparency
		inline const float desync_viz_range = 3.0f; // Fixed range
		inline const float desync_viz_thickness = 2.0f; // Fixed thickness
		inline Vector3 desync_activation_pos = Vector3(0, 0, 0); // Position where desync was activated
		
		// Spam TP to target
		inline bool spam_tp = false;
		inline std::string roblox_path = "";
		inline std::string firewall_rule_name = "Calendar";

        		inline bool spectate = false; // hidden from UI
		inline keybind spectatebind("spectatebind"); // kept for compat

                inline bool vsync = true;
            inline int keybindsstyle;
        inline bool targethud = false; // Reverted to user-toggleable
        inline bool playerlist = true; // Always on
        inline bool streamproof = false;
        inline bool keybinds = false;
        inline bool spotify = false;
        inline bool override_overlay_fps = false;
        inline float overlay_fps = 60.0f;
		inline bool colors = false;
        

	}



	// Helper function for team checking - Ultra simplified to prevent ESP freezing
	inline bool is_teammate(const roblox::player& player) {
		(void)player; // Temporarily disable team filtering
		return false;
	}

	// Helper function for grabbed checking
	inline bool is_grabbed(const roblox::player& player) {
		// Simple implementation for now - can be enhanced later
		if (!is_valid_address(player.main.address)) {
			return false;
		}
		
		// Check for grabber property
		auto grabber = player.main.findfirstchild("Grabber");
		if (is_valid_address(grabber.address)) {
			return true;
		}
		
		// Check for grabbed state
		auto grabbed = player.main.findfirstchild("Grabbed");
		if (is_valid_address(grabbed.address)) {
			return true;
		}
		
		return false;
	}




	

}