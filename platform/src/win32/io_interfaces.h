//taken from bsnes / ruby by byuu

#pragma once

#include <ddraw.h>

#include <mmsystem.h>
#include "dsound.h"
#include "stdint.h"

//typedef unsigned long uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;
namespace io
{
	namespace input
	{
	}
	namespace sound
	{
		struct DirectSoundAudio
		{
			LPDIRECTSOUND ds;
			LPDIRECTSOUNDBUFFER dsb_p, dsb_b;
			DSBUFFERDESC dsbd;
			WAVEFORMATEX wfx;

			struct {
				unsigned rings;
				unsigned latency;

				uint32_t *buffer;
				unsigned bufferoffset;

				unsigned readring;
				unsigned writering;
				int distance;
			} device;

			struct {
				HWND handle;
				bool synchronize;
				unsigned frequency;
				unsigned latency;
			} settings;

			void SetSample(uint16_t left, uint16_t right) {
				device.buffer[device.bufferoffset++] = left + (right << 16);
				if(device.bufferoffset < device.latency) return;
				device.bufferoffset = 0;

				DWORD pos, size;
				void *output;

				if(settings.synchronize == true) {
					//wait until playback buffer has an empty ring to write new audio data to
					while(device.distance >= device.rings - 1) {
						dsb_b->GetCurrentPosition(&pos, 0);
						unsigned activering = pos / (device.latency * 4);
						if(activering == device.readring) {
							if(settings.synchronize == false) Sleep(1);
							continue;
						}

						//subtract number of played rings from ring distance counter
						device.distance -= (device.rings + activering - device.readring) % device.rings;
						device.readring = activering;

						if(device.distance < 2) {
							//buffer underflow; set max distance to recover quickly
							device.distance  = device.rings - 1;
							device.writering = (device.rings + device.readring - 1) % device.rings;
							break;
						}
					}
				}

				device.writering = (device.writering + 1) % device.rings;
				device.distance  = (device.distance  + 1) % device.rings;

				if(dsb_b->Lock(device.writering * device.latency * 4, device.latency * 4, &output, &size, 0, 0, 0) == DS_OK) {
					memcpy(output, device.buffer, device.latency * 4);
					dsb_b->Unlock(output, size, 0, 0);
				}
			}

			void Clear() {
				device.readring  = 0;
				device.writering = device.rings - 1;
				device.distance  = device.rings - 1;

				device.bufferoffset = 0;
				if(device.buffer) memset(device.buffer, 0, device.latency * device.rings * 4);

				if(!dsb_b) return;
				dsb_b->Stop();
				dsb_b->SetCurrentPosition(0);

				DWORD size;
				void *output;
				dsb_b->Lock(0, device.latency * device.rings * 4, &output, &size, 0, 0, 0);
				memset(output, 0, size);
				dsb_b->Unlock(output, size, 0, 0);

				dsb_b->Play(0, 0, DSBPLAY_LOOPING);
			}
			void DeInitialize() {
				if(device.buffer) {
					delete[] device.buffer;
					device.buffer = 0;
				}

				if(dsb_b) { dsb_b->Stop(); dsb_b->Release(); dsb_b = 0; }
				if(dsb_p) { dsb_p->Stop(); dsb_p->Release(); dsb_p = 0; }
				if(ds) { ds->Release(); ds = 0; }
			}
			bool Initialize() {
				DeInitialize();

				device.rings   = 8;
				device.latency = settings.frequency * settings.latency / device.rings / 1000.0 + 0.5;
				device.buffer  = new uint32_t[device.latency * device.rings];
				device.bufferoffset = 0;

				DirectSoundCreate(0, &ds, 0);
				ds->SetCooperativeLevel((HWND)settings.handle, DSSCL_PRIORITY);

				memset(&dsbd, 0, sizeof(dsbd));
				dsbd.dwSize        = sizeof(dsbd);
				dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
				dsbd.dwBufferBytes = 0;
				dsbd.lpwfxFormat   = 0;
				ds->CreateSoundBuffer(&dsbd, &dsb_p, 0);

				memset(&wfx, 0, sizeof(wfx));
				wfx.wFormatTag      = WAVE_FORMAT_PCM;
				wfx.nChannels       = 2;
				wfx.nSamplesPerSec  = settings.frequency;
				wfx.wBitsPerSample  = 16;
				wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
				wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
				dsb_p->SetFormat(&wfx);

				memset(&dsbd, 0, sizeof(dsbd));
				dsbd.dwSize  = sizeof(dsbd);
				dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE;
				dsbd.dwBufferBytes   = device.latency * device.rings * sizeof(uint32_t);
				dsbd.guid3DAlgorithm = GUID_NULL;
				dsbd.lpwfxFormat     = &wfx;
				ds->CreateSoundBuffer(&dsbd, &dsb_b, 0);
				dsb_b->SetFrequency(settings.frequency);
				dsb_b->SetCurrentPosition(0);

				Clear();
				return true;
			}



			DirectSoundAudio() {
				ds = 0;
				dsb_p = 0;
				dsb_b = 0;

				device.buffer = 0;
				device.bufferoffset = 0;
				device.readring = 0;
				device.writering = 0;
				device.distance = 0;

				settings.handle = GetDesktopWindow();
				settings.synchronize = false;
				settings.frequency = 44100;
				settings.latency = 30;
			}
		};
	}
	namespace video
	{
		struct DirectDrawVideo
		{
			LPDIRECTDRAW lpdd;
			LPDIRECTDRAW7 lpdd7;
			LPDIRECTDRAWSURFACE7 screen, raster;
			LPDIRECTDRAWCLIPPER clipper;
			DDSURFACEDESC2 ddsd;
			DDSCAPS2 ddscaps;
			unsigned iwidth, iheight;

			struct {
				HWND handle;
				bool synchronize;

				unsigned width;
				unsigned height;
			} settings;

			void Resize(unsigned width, unsigned height) {
				if(iwidth >= width && iheight >= height) return;

				iwidth  = max(width,  iwidth);
				iheight = max(height, iheight);

				if(raster) raster->Release();

				screen->GetSurfaceDesc(&ddsd);
				int depth = ddsd.ddpfPixelFormat.dwRGBBitCount;
				if(depth == 32) goto try_native_surface;

				memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
				ddsd.dwSize = sizeof(DDSURFACEDESC2);
				ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;  //DDSCAPS_SYSTEMMEMORY
				ddsd.dwWidth  = iwidth;
				ddsd.dwHeight = iheight;

				ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
				ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
				ddsd.ddpfPixelFormat.dwRBitMask = 0xff0000;
				ddsd.ddpfPixelFormat.dwGBitMask = 0x00ff00;
				ddsd.ddpfPixelFormat.dwBBitMask = 0x0000ff;

				if(lpdd7->CreateSurface(&ddsd, &raster, 0) == DD_OK) return Clear();

try_native_surface:
				memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
				ddsd.dwSize = sizeof(DDSURFACEDESC2);
				ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;  //DDSCAPS_SYSTEMMEMORY
				ddsd.dwWidth  = iwidth;
				ddsd.dwHeight = iheight;

				if(lpdd7->CreateSurface(&ddsd, &raster, 0) == DD_OK) return Clear();
			}

			void Clear() {
				DDBLTFX fx;
				fx.dwSize = sizeof(DDBLTFX);
				fx.dwFillColor = 0x00000000;
				screen->Blt(0, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
				raster->Blt(0, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
			}
#if 1
			bool Lock(uint32_t *&data, unsigned &pitch, unsigned width, unsigned height) {
				if(width != settings.width || height != settings.height) {
					Resize(settings.width = width, settings.height = height);
				}

				if(raster->Lock(0, &ddsd, DDLOCK_WAIT, 0) != DD_OK) {
					raster->Restore();
					if(raster->Lock(0, &ddsd, DDLOCK_WAIT, 0) != DD_OK) return false;
				}
				pitch = ddsd.lPitch;
				return data = (uint32_t*)ddsd.lpSurface;
			}
#endif
			void Unlock() {
				raster->Unlock(0);
			}

			bool Initialize() {
				DeInitialize();

				DirectDrawCreate(0, &lpdd, 0);
				lpdd->QueryInterface(IID_IDirectDraw7, (void**)&lpdd7);
				if(lpdd) { lpdd->Release(); lpdd = 0; }

				lpdd7->SetCooperativeLevel(settings.handle, DDSCL_NORMAL);

				memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
				ddsd.dwSize = sizeof(DDSURFACEDESC2);

				ddsd.dwFlags = DDSD_CAPS;
				ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
				lpdd7->CreateSurface(&ddsd, &screen, 0);

				lpdd7->CreateClipper(0, &clipper, 0);
				clipper->SetHWnd(0, settings.handle);
				screen->SetClipper(clipper);

				raster  = 0;
				iwidth  = 0;
				iheight = 0;
				Resize(settings.width = 256, settings.height = 256);

				return true;
			}

			void DeInitialize() {
				if(clipper) { clipper->Release(); clipper = 0; }
				if(raster) { raster->Release(); raster = 0; }
				if(screen) { screen->Release(); screen = 0; }
				if(lpdd7) { lpdd7->Release(); lpdd7 = 0; }
				if(lpdd) { lpdd->Release(); lpdd = 0; }
			}
			void Refresh() {
				if(settings.synchronize) {
					while(true) {
						BOOL in_vblank;
						lpdd7->GetVerticalBlankStatus(&in_vblank);
						if(in_vblank == true) break;
					}
				}

				RECT rd, rs;
				SetRect(&rs, 0, 0, settings.width, settings.height);

				POINT p = { 0, 0 };
				ClientToScreen(settings.handle, &p);
				GetClientRect(settings.handle, &rd);
				OffsetRect(&rd, p.x, p.y);

				if(screen->Blt(&rd, raster, &rs, DDBLT_WAIT, 0) == DDERR_SURFACELOST) {
					screen->Restore();
					raster->Restore();
				}
			}
			DirectDrawVideo() {
				lpdd = 0;
				lpdd7 = 0;
				screen = 0;
				raster = 0;
				clipper = 0;

				settings.handle = 0;
			}
		};
	}
}