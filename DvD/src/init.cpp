#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif

#include "fighter.h"
#include "graphics.h"
#include "error.h"
#include "menu.h"
#include "sound.h"
#include "stage.h"
#include "network.h"
#include "file.h"
#include "input.h"
#include "effect.h"

std::string szConfigPath;

static bool disable_shaders = false;
static bool disable_sound = false;
static int max_texture_size = 0;
static bool versus = false;
static bool fullscreen = false;
static int input_delay = 0;

void parseArgs(int argc, char** argv) {
	extern game::Player madotsuki;
	extern game::Player poniko;

	madotsuki.fighter = &game::fighters[0];
	poniko.fighter = &game::fighters[0];

	for(int i = 1; i < argc; i++) {
		if(!strcasecmp(argv[i], "--disable-shaders")) {
			disable_shaders = true;
		} else if(!strcasecmp(argv[i], "--disable-sound")) {
			disable_sound = true;
		} else if(!strcasecmp(argv[i], "--training")) {
			menu = MENU_FIGHT;
		} else if(!strcasecmp(argv[i], "--versus")) {
			menu = MENU_FIGHT;
			versus = true;
		} else if(!strcasecmp(argv[i], "--fullscreen")) {
			fullscreen = true;
		} else if(!strcasecmp(argv[i], "-char1")) {
			if(++i < argc) {
				madotsuki.fighter = &game::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-char2")) {
			if(++i < argc) {
				poniko.fighter = &game::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-pal1")) {
			if(++i < argc) {
				madotsuki.palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-pal2")) {
			if(++i < argc) {
				poniko.palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-stage")) {
			if(++i < argc) {
				stage = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-input-delay")) {
			if(++i < argc) {
				input_delay = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-max-texture-size")) {
			if(++i < argc) {
				max_texture_size = atoi(argv[i]);
			}
		}
	}
}

#define OPTIONS_VERSION 0

void optionsLoad() {
	File file;
	if(!file.open(FILE_READ_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

    //If the options are newer, don't read them
    if((ubyte_t)file.readByte() > OPTIONS_VERSION) {
        return;
    }

	optionDifficulty = file.readByte();
	optionWins = file.readByte();
	optionTime = file.readByte();
	optionSfxVolume = file.readByte();
	optionMusVolume = file.readByte();
	optionVoiceVolume = file.readByte();
	optionEpilepsy = file.readByte();
}

void optionsSave() {
	File file;
	if(!file.open(FILE_WRITE_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

    file.writeByte(OPTIONS_VERSION);
	file.writeByte(optionDifficulty);
	file.writeByte(optionWins);
	file.writeByte(optionTime);
	file.writeByte(optionSfxVolume);
	file.writeByte(optionMusVolume);
	file.writeByte(optionVoiceVolume);
	file.writeByte(optionEpilepsy);
}

void init() {
	void deinit();
	atexit(deinit);

	srand(time(nullptr));

	//Create settings path first
	//Find correct directory
#if defined _WIN32
#define CONFIG_PATH "\\dreamvsdream\\"
	wchar_t* env16 = _wgetenv(L"AppData");
	if(!env16) {
		die("Cannot find settings directory.");
	}
	char* env = util::utf16to8(env16);
#elif defined __APPLE__
#define CONFIG_PATH "/Library/Application Support/dreamvsdream/"
	char* env = getenv("HOME");
	if(!env) {
		die("Cannot find settings directory.");
	}
#else
#define CONFIG_PATH "/dreamvsdream/"
	char* env = getenv("XDG_CONFIG_HOME");
	if(!env) {
		env = getenv("HOME");
		if(!env) {
			die("Cannot find settings directory.");
		}
	}
#endif

	//Allocate into new string
	szConfigPath = std::string(env) + CONFIG_PATH;

	//Create directory if it doesn't exist
#ifdef _WIN32
	free(env);
	wchar_t* szConfigPath16 = util::utf8to16(szConfigPath.c_str());
	CreateDirectoryW(szConfigPath16, nullptr);
	free(szConfigPath16);
#else
	mkdir(szConfigPath.c_str(), 0777);
#endif

	optionsLoad();

	os::init();
	graphics::init(disable_shaders, max_texture_size);
	extern Image imgLoading;
	imgLoading.createFromFile("menus/loading.png");
	imgLoading.draw(0, 0);
	os::refresh();

	input::init();
	if(!disable_sound) {
		audio::init();
	}
	net::init(input_delay);
	game::init();
	Menu::ginit(); //TODO fix this
	Stage::ginit();
	effect::init();

	if(versus) {
		FIGHT->gametype = GAMETYPE_VERSUS;
	}
	//if(fullscreen)
	//OS::toggleFullscreen();

	if(menu == MENU_FIGHT && stage == -1) {
		stage = 0;
	}
}

void deinit() {
    audio::deinit();

	effect::deinit();
	Stage::deinit();
	Menu::deinit();
	game::deinit();
	net::deinit();

	graphics::deinit();
	os::deinit();

	optionsSave();
}
