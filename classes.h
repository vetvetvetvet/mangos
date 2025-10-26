#pragma once

#include "math/math.h"
#include <iostream>
#include <vector>
#include "../../drawing/imgui/imgui.h"
#include "../driver/driver.h"
#include "../offsets.h"
using namespace math;

namespace roblox {
	class instance {
	public:
		uintptr_t address;
		static std::uint64_t cached_input_object;
		bool operator==(const roblox::instance& get) const {
			return this->address == get.address;
		}

		bool is_valid() const noexcept {
			return address != 0;
		}

		math::Vector2 GetDimensins();

		math::Matrix4 GetViewMatrix();

		void setAnchored(bool booz);
		uintptr_t primitive() const;
		//setFramePosX//setFramePosY//initialize_mouseservice//write_mouse_position

		void setFramePosX(uint64_t pos);
		void setFramePosY(uint64_t pos);
		void initialize_mouse_service(std::uint64_t address);
		void write_mouse_position(std::uint64_t address, float x, float y);
		std::string get_name() const;
		float read_health() const;
		void write_health(float health);
		float read_maxhealth() const;
		std::string get_displayname() const;
		std::string get_humdisplayname() const;
		std::string get_class_name() const;

		std::vector<instance> get_children() const;

		instance findfirstchild(std::string arg) const;
		instance read_service(std::string arg) const;
		void write_animationid(std::string id);
		std::string read_animationid() const;
		instance read_parent() const;

		Vector3 get_pos() const;
		Vector3 get_velocity() const;

		void write_position(Vector3 pos);
		void write_velocity(Vector3 velocity);

		int read_color() const;
		void write_color(int col);

		void write_ambiance(Vector3 col);

		void write_fogcolor(Vector3 col);

		void write_fogstart(float distance);

		void write_fogend(float distance);

		void write_cancollide(bool boolleanz);
		bool get_cancollide() const;
		Vector3 get_cam_pos() const;
		Matrix3 get_cam_rot() const;

		void write_cam_pos(Vector3 arg);
		void write_cam_rot(Matrix3 arg);

		instance model_instance() const;
		instance local_player() const;

		roblox::instance read_workspace() const;

		Vector2 get_dimensions() const;

		Vector3 read_vector3_value() const;
		int read_int_value() const;
		bool read_bool_value() const;

		void write_vector3_value(Vector3 arg);
		void write_float_value(float val);
		double read_double_value() const;
		void write_double_value(double val);
		float read_float_value() const;
		void write_int_value(int arg);
		void write_bool_value(bool arg);

		roblox::instance read_userid() const;

		//void writeprimgrav(float gravity);

		float read_primgrav() const;

		// World to screen conversion
		static math::Vector2 worldtoscreen(const math::Vector3& world_pos);

		float read_walkspeed() const;
		void write_walkspeed(float value);
		float read_jumppower() const;
		void write_jumppower(float value);
		float read_hipheight() const;
		void write_hipheight(float value);
		bool read_sitting() const;
		void write_sitting(bool value);
		Vector3 get_part_size() const;

		void write_part_size(Vector3 size);

		Vector3 get_move_dir() const;

		void write_move_dir(Vector3 size);

		CFrame read_cframe() const;
		Matrix3 read_part_cframe() const;
		void write_cframe(CFrame cframe);
		void write_part_cframe(Matrix3 ma3);
		void spectate(uint64_t stringhere);
		void unspectate();

		// fetch entity
		int fetch_entity(std::uint64_t address) const;
		
		// Team method for team checking
		roblox::instance Team() const;
		
		// Team checking method
		bool is_same_team(const roblox::instance& other_player) const;
		
		// Helper method to read team name
		std::string read_team_name() const;
	};

	class wall {
	public:
		Vector3 size;
		CFrame cframe;
		Vector3 position;
		Matrix3 rotation;
	};

	class player {
	public:
		uintptr_t address;
		bool operator==(const roblox::player& get) const {
			return this->address == get.address;
		}

		int rigtype = 0;
		roblox::instance torso;
	//	std::unordered_map<std::string, roblox::instance> cachedchildren;
		roblox::instance head;
		roblox::instance hrp;
		roblox::instance uppertorso;
		roblox::instance lowertorso;
		roblox::instance rightupperarm;
		roblox::instance leftupperarm;
		roblox::instance rightlowerarm;
		roblox::instance leftlowerarm;
		roblox::instance lefthand;
		roblox::instance righthand;
		roblox::instance leftupperleg;
		roblox::instance rightupperleg;
		roblox::instance rightlowerleg;
		roblox::instance leftlowerleg;
		roblox::instance rightfoot;
		roblox::instance leftfoot;
		roblox::instance humanoid;
		roblox::instance main;
		std::string flag;
		bool pictureDownloaded = false;
		roblox::instance team;
		roblox::instance instance;
		int maxhealth;
		int health;
		int armor;
		roblox::instance bodyeffects;
		std::vector<roblox::instance> bones;
		std::string name;
		std::string displayname;
		std::string toolname;
		roblox::instance tool;
		roblox::instance userid;
		int status;
	};

	class camera {
	public:
		uintptr_t address;

		bool operator==(const roblox::camera& get) const {
			return this->address == get.address;
		}


		Vector3 getPos();

		Matrix3 getRot();

		void writePos(Vector3 arg);

		void writeRot(Matrix3 arg);

		int getType();

		void setType(int type);

		roblox::instance getSubject();

		int getMode();

		void setMode(int type);

	};
	Vector2 worldtoscreen(Vector3 world);
	inline std::uint64_t cached_input_object;

	void cached_input_objectzz();
	uint32_t Vector3ToIntBGR(Vector3 color);
}
namespace engine {
	bool startup();

}

// Custom Models functions
std::vector<roblox::player> GetCustomModels(uintptr_t baseAddress);
roblox::instance ResolveRobloxPath(const std::string& path);