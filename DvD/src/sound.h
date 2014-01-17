#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#ifdef EMSCRIPTEN
#include <SDL/SDL_mixer.h>
#endif

#include <string>
#include "globals.h"

class Sound
{
public:
    Sound();
    ~Sound();
    void play();
    void play(float freq);
    void stop();

    bool playing();

    void createFromFile(std::string filename);
    bool exists();
    void destroy();

#ifdef EMSCRIPTEN
    Mix_Chunk* sound;
    int channel;
#else
    float* samples;
    unsigned int c_samples;
    unsigned int sample_rate;
    int channels;
#endif

    //Init/deinit
    static void init();
    static void deinit();

    static void refresh();

    static bool enabled;
};

#define AUDIO_STREAM_BUFFER_MAX 3

class Music
{
public:
	Music();
	~Music();

	void play();

	static void stop();

	void createFromFile(std::string szIntro, std::string szLoop);
	bool exists();

	bool isPlaying();

#ifdef EMSCRIPTEN
	Mix_Music* intro;
	Mix_Music* loop;
#else
	Sound intro;
	Sound loop;
#endif
};

/*
class Voice
{
public:
    Voice();
    ~Voice();

    void createFromFile(const char* filename);
    bool exists();

    Sound sound;
};
*/

typedef Sound Voice;

class Speaker
{
public:
    Speaker();
    ~Speaker();

#ifdef EMSCRIPTEN
    int channel;
#endif

    void init();
    void play(Voice* voice, bool randomize = false);
    void stop();
};


#endif // SOUND_H_INCLUDED
