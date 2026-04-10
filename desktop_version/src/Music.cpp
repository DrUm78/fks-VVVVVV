#include <SDL.h>
#include <stdio.h>
#include <cstdlib>	// for getenv
#include <string>	// for std::string
#include "Music.h"
#include "BinaryBlob.h"

musicclass::musicclass()
{
	// Load sound effects (WAV)
	soundTracks.push_back(SoundTrack( "sounds/jump.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/jump2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/hurt.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/souleyeminijingle.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/coin.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/save.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crumble.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/vanish.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/blip.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/preteleport.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/teleport.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew1.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew3.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew4.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew5.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew6.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/terminal.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/gamesaved.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crashing.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/blip2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/countdown.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/go.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crash.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/combine.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/newrecord.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/trophy.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/rescue.wav" ));

	// Build the path to MP3 files using $HOME/.local/share/VVVVVV/music/
	const char* homeDir = getenv("HOME");
	std::string musicPath;
	if (homeDir) {
		musicPath = std::string(homeDir) + "/.local/share/VVVVVV/music/";
	} else {
		// Fallback to a relative path if $HOME is not set (rare case)
		musicPath = "./music/";
		fprintf(stderr, "Warning: $HOME not set, using ./music/ instead\n");
	}

	// List of MP3 tracks
	const char* mp3Filenames[16] = {
		"0levelcomplete.mp3",
		"1pushingonwards.mp3",
		"2positiveforce.mp3",
		"3potentialforanything.mp3",
		"4passionforexploring.mp3",
		"5intermission.mp3",
		"6presentingvvvvvv.mp3",
		"7gamecomplete.mp3",
		"8predestinedfate.mp3",
		"9positiveforcereversed.mp3",
		"10popularpotpourri.mp3",
		"11pipedream.mp3",
		"12pressurecooker.mp3",
		"13pacedenergy.mp3",
		"14piercingthesky.mp3",
		"predestinedfatefinallevel.mp3"
	};

	// Load MP3 files from $HOME/.local/share/VVVVVV/music/
	binaryBlob musicReadBlob;
	if (!musicReadBlob.unPackBinary("mmmmmm.vvv"))
	{
		mmmmmm = false;
		usingmmmmmm = false;
		bool ohCrap = musicReadBlob.unPackBinary("vvvvvvmusic.vvv");
	}
	else
	{
		mmmmmm = true;
		usingmmmmmm = true;
	}

	// Load the 16 MP3 tracks from the constructed path
	for (int i = 0; i < 16; i++) {
		std::string fullPath = musicPath + mp3Filenames[i];
		SDL_RWops *rw = SDL_RWFromFile(fullPath.c_str(), "rb");
		if (!rw) {
			fprintf(stderr, "Failed to open MP3 file: %s\n", fullPath.c_str());
			musicTracks.push_back(MusicTrack((SDL_RWops *)NULL));
		} else {
			musicTracks.push_back(MusicTrack(rw));
		}
	}

	// Initialize variables
	safeToProcessMusic = false;
	m_doFadeInVol = false;
	musicVolume = 128;
	FadeVolAmountPerFrame = 0;
	custompd = false;
}

void musicclass::play(int t)
{
	t = (t % 16);

	if(mmmmmm)
	{
		if(!usingmmmmmm)
		{
			t += 16;
		}
	}
	safeToProcessMusic = true;
	Mix_VolumeMusic(128);
	if (currentsong != t)
	{
		if (currentsong != -1)
		{
			Mix_HaltMusic();
		}
		if (t != -1)
		{
			currentsong = t;
			if (currentsong == 0 || currentsong == 7)
			{
				if (Mix_FadeInMusic(musicTracks[t].m_music, 0, 0) == -1)
				{
					printf("Mix_PlayMusic: %s\n", Mix_GetError());
				}
			}
			else
			{
				if (Mix_FadeInMusic(musicTracks[t].m_music, -1, 3000) == -1)
				{
					printf("Mix_FadeInMusic: %s\n", Mix_GetError());
				}
			}
		}
		else
		{
			currentsong = -1;
		}
	}
}

void musicclass::loopmusic()
{
	// Empty function (logic handled by SDL_mixer)
}

void musicclass::stopmusic()
{
	Mix_HaltMusic();
	currentsong = -1;
}

void musicclass::haltdasmusik()
{
	Mix_HaltMusic();
	currentsong = -1;
}

void musicclass::silencedasmusik()
{
	Mix_VolumeMusic(0);
	musicVolume = 0;
}

void musicclass::fadeMusicVolumeIn(int ms)
{
	m_doFadeInVol = true;
	FadeVolAmountPerFrame = MIX_MAX_VOLUME / (ms / 33);
}

void musicclass::fadeout()
{
	Mix_FadeOutMusic(2000);
	currentsong = -1;
}

void musicclass::processmusicfade()
{
	// Empty function (logic handled by SDL_mixer)
}

void musicclass::processmusicfadein()
{
	musicVolume += FadeVolAmountPerFrame;
	Mix_VolumeMusic(musicVolume);
	if (musicVolume >= MIX_MAX_VOLUME)
	{
		m_doFadeInVol = false;
	}
}

void musicclass::processmusic()
{
	if(!safeToProcessMusic)
	{
		return;
	}

	if (nicefade == 1 && Mix_PlayingMusic() == 0)
	{
		play(nicechange);
		nicechange = -1;
		nicefade = 0;
	}

	if(m_doFadeInVol)
	{
		processmusicfadein();
	}
}

void musicclass::niceplay(int t)
{
	if(currentsong != t)
	{
		if(currentsong != -1) fadeout();
		nicefade = 1;
		nicechange = t;
	}
}

void musicclass::changemusicarea(int x, int y)
{
	switch(musicroom(x, y))
	{
	case musicroom(11, 4):
		niceplay(2);
		break;

	case musicroom(2, 4):
	case musicroom(7, 15):
		niceplay(3);
		break;

	case musicroom(18, 1):
	case musicroom(15, 0):
		niceplay(12);
		break;

	case musicroom(0, 0):
	case musicroom(0, 16):
	case musicroom(2, 11):
	case musicroom(7, 9):
	case musicroom(8, 11):
	case musicroom(13, 2):
	case musicroom(17, 12):
	case musicroom(14, 19):
	case musicroom(17, 17):
		niceplay(4);
		break;

	default:
		niceplay(1);
		break;
	}
}

void musicclass::initefchannels()
{
	// Empty function (logic handled by SDL_mixer)
}

void musicclass::playef(int t, int offset)
{
	int channel = Mix_PlayChannel(-1, soundTracks[t].sound, 0);
	if(channel == -1)
	{
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}
