#include "SDL/SDL.h"

#include "stdio.h"

#include "../../../lib/c/saturnki.h"
#include "../../../src/saturn_input.h"

#include "../common/cdbase.h"

SDL_Surface * sdl_surface;

void InitSdl()
{
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		printf("Error trying to initialize SDL: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_WM_SetCaption("saturnki", NULL);

	//we don't have c99 on msvc so we use braces instead.
	{
		const SDL_VideoInfo * sdl_video_info = SDL_GetVideoInfo( );

		if ( !sdl_video_info ) {
			printf("Video query failed: %s\n", SDL_GetError( ) );
			exit( -1);
		}

		{
			Uint32 sdl_video_flags = 0;

			if ( sdl_video_info->blit_hw )
				sdl_video_flags |= SDL_HWACCEL;

			sdl_video_flags |= SDL_SWSURFACE;

			sdl_surface = SDL_SetVideoMode(704, 480, 32, sdl_video_flags);
		}
	}

	if ( !sdl_surface ) {
		printf("Video mode set failed: %s\n", SDL_GetError());
		exit(-1);
	}
}

void VideoUpdate(const uint32_t *data, const unsigned width, const unsigned height) {

	SDL_Surface *saturn_framebuffer;
	int pitch = sizeof(uint32_t) * width;

	saturn_framebuffer = SDL_CreateRGBSurfaceFrom((void*)data, width, height, 32, pitch, 0x00FF, 0xFF00, 0xFF0000, 0);

	if(saturn_framebuffer == NULL) return;

	SDL_BlitSurface(saturn_framebuffer, 0, sdl_surface, 0);

	SDL_UpdateRect(sdl_surface, 0, 0, 0, 0);

	SDL_FreeSurface(saturn_framebuffer);
}

void AudioUpdate(const uint16_t left, const uint16_t right)
{
}

struct SaturnStandardPadInputType InputUpdate()
{
	struct SaturnStandardPadInputType s;
	return s;
}

void InitSaturn(char*image_filename)
{
	SaturnSetVideoUpdateCallback(VideoUpdate);
	SaturnSetAudioSampleUpdateCallback(AudioUpdate);
	SaturnSetInputUpdateCallback(InputUpdate);

	SaturnSetCdGetPhysicalStatusCallback(GetPhysicalStatus);
	SaturnSetCdReadTocCallback(ReadToc);
	SaturnSetCdReadSectorAtFadCallback(ReadSectorAtFad);

	CdInit("C:\\SaturnOrbit\\EXAMPLES\\CharlesMacDonald\\satwin2\\main_coff.iso");
	SaturnPressPowerButton();
}

#ifdef WIN32
#include "windows.h"
#include <io.h>
#include <fcntl.h>
void OpenConsole() 
{
	AllocConsole();
	{
		//redirect stdio
		long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		FILE *fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
	}
}
#else
void OpenConsole() {}
#endif

void CountFps()
{
	static unsigned frame_counter = 0;
	static Uint32 time_counter = 0;

	Uint32 current_time = SDL_GetTicks() / 1000;

	frame_counter++;

	if(time_counter != current_time)
	{
		char buf[100];

		time_counter = current_time;

		sprintf(buf,"saturnki - fps: %d",frame_counter);
		SDL_WM_SetCaption(buf, NULL);
		frame_counter = 0;
	}
}

int main( int argc, char* argv[] )
{
	SDL_Event sdl_event;

	InitSdl();

	//OpenConsole();

	InitSaturn(argv[1]);

	while(1)
	{
		while(SDL_PollEvent(&sdl_event)) {
			switch(sdl_event.type){
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if ( sdl_event.key.keysym.sym == SDLK_ESCAPE )
					goto finished;
				break;
			case SDL_QUIT:
				goto finished;
			}
		}

		CountFps();

		SaturnRunFrame();
		SDL_Flip(sdl_surface);
	}
finished:
	return 0;
}
