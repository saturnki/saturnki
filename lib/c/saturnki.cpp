#define BUILDING_DLL
#include "saturnki.h"

#include "../../src/saturn/saturn.h"

struct MySaturn : public saturn::Saturn
{
	//callback pointers
	SaturnVideoUpdateCallbackType SaturnVideoUpdateCallback;
	SaturnAudioSampleUpdateCallbackType SaturnAudioSampleUpdateCallback;
	SaturnInputUpdateCallbackType SaturnInputUpdateCallback;

	SaturnCdGetPhysicalStatusCallbackType SaturnCdGetPhysicalStatusCallback;
	SaturnCdReadTocCallbackType SaturnCdReadTocCallback;
	SaturnCdReadSectorAtFadCallbackType SaturnCdReadSectorAtFadCallback;

	//the callbacks
	void InterfaceUpdateAudioSample(const s16 &l, const s16& r) 
	{
		if(SaturnAudioSampleUpdateCallback)
			return SaturnAudioSampleUpdateCallback(l,r);
		else
			assert("SaturnAudioSampleUpdateCallback is null");
	}
	void InterfaceUpdateFramebuffer(const u32 *data, const u32&width, const u32&height)
	{
		u32* d = const_cast<u32*>(data);
		if(SaturnVideoUpdateCallback)
			return SaturnVideoUpdateCallback(reinterpret_cast<uint32_t*>(d),width,height);
		else
			assert("SaturnVideoUpdateCallback is null");
	}
	SaturnStandardPadInputType InterfaceUpdateInput() 
	{
		if(SaturnInputUpdateCallback)
			return SaturnInputUpdateCallback();
		else
			assert("SaturnInputUpdateCallback is null");

		assert(false);
		SaturnStandardPadInputType s = {0};
		return s;
	}

	int InterfaceCdGetPhysicalStatus()
	{
		if(SaturnCdGetPhysicalStatusCallback)
			return SaturnCdGetPhysicalStatusCallback();
		else
			assert("SaturnCdGetPhysicalStatusCallback is null");

		return 0;
	}

	s32 InterfaceCdReadToc(u32* toc)
	{
		if(SaturnCdReadTocCallback)
			return SaturnCdReadTocCallback(reinterpret_cast<uint32_t*>(toc));
		else
			assert("SaturnCdReadTocCallback is null");

		return 0;
	}

	int InterfaceCdReadSectorAtFad(u32 fad, void *buffer)
	{
		if(SaturnCdReadSectorAtFadCallback)
			return SaturnCdReadSectorAtFadCallback(fad,buffer);
		else
			assert("SaturnCdReadSectorAtFadCallback is null");

		return 0;
	}

}saturn_impl;


//callback setting
void SaturnSetVideoUpdateCallback(SaturnVideoUpdateCallbackType s)
{
	saturn_impl.SaturnVideoUpdateCallback = s;
}
void SaturnSetAudioSampleUpdateCallback(SaturnAudioSampleUpdateCallbackType s)
{
	saturn_impl.SaturnAudioSampleUpdateCallback = s;
}
void SaturnSetInputUpdateCallback(SaturnInputUpdateCallbackType s)
{
	saturn_impl.SaturnInputUpdateCallback = s;
}

//cd functions
void SaturnSetCdGetPhysicalStatusCallback(SaturnCdGetPhysicalStatusCallbackType s)
{
	saturn_impl.SaturnCdGetPhysicalStatusCallback = s;
}

void SaturnSetCdReadTocCallback(SaturnCdReadTocCallbackType s)
{
	saturn_impl.SaturnCdReadTocCallback = s;
}

void SaturnSetCdReadSectorAtFadCallback(SaturnCdReadSectorAtFadCallbackType s)
{
	saturn_impl.SaturnCdReadSectorAtFadCallback = s;
}


//various ways to interact with the saturn
void SaturnPressPowerButton()
{
	saturn_impl.InterfacePowerOn();
}

void SaturnPressResetButton()
{
}

void SaturnOpenCdTray()
{
}

void SaturnCloseCdTray()
{
}

void SaturnSaveState()
{
}

void SaturnLoadState()
{
}
void SaturnRunFrame()
{
	saturn_impl.NewExecute();
}
