#pragma once
#include "../util/classes/classes.h"
#include "../util/globals.h"

using namespace engine;


class player_cache {
public:
	player_cache() { Initialize(); }
	~player_cache() { Cleanup(); }
	void Initialize();
	void UpdateCache();
	void Cleanup();
	void Refresh();
};



namespace hooks {
	void listener();
	void anti_aim();
	void cache();
	void silentrun();
	void silentrun2();
	void silent_combined();
	void silent();
	void speed();
	void jumppower();
	void flight();
	void hipheight();
	void antisit();
	void combat();
	void misc();
	void overlay();
	void autoreload();
	void autoarmor();
	void spinbot();
	void rapidfire();
	void autostomp();
	void antistomp();
	void voidhide();
	void orbit();
	void triggerbot();
	void aimbot();
	
	// Animation and enhanced movement hooks
	void animationchanger();
	void cframe();
	void antiaim();
	void desync();
	void spam_tp();

}


namespace hooking {
	void launchThreads();
}