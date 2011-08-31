#ifndef SATURNKI_H
#define SATURNKI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif //BUILDING_DLL
#else //!WIN32
#define DLL_EXPORT
#endif //WIN32

#include <stdint.h>

//callback type signatures
typedef void (*SaturnVideoUpdateCallbackType)(const uint32_t *data, const unsigned width, const unsigned height);
typedef void (*SaturnAudioSampleUpdateCallbackType)(const uint16_t left, const uint16_t right);
typedef struct SaturnStandardPadInputType (*SaturnInputUpdateCallbackType)();

//cd funcs
typedef int (*SaturnCdGetPhysicalStatusCallbackType)();
typedef int32_t (*SaturnCdReadTocCallbackType)(uint32_t* toc);
typedef int (*SaturnCdReadSectorAtFadCallbackType)(uint32_t fad, void *buffer);

//callback setting functions
DLL_EXPORT void SaturnSetVideoUpdateCallback(SaturnVideoUpdateCallbackType s);
DLL_EXPORT void SaturnSetAudioSampleUpdateCallback(SaturnAudioSampleUpdateCallbackType s);
DLL_EXPORT void SaturnSetInputUpdateCallback(SaturnInputUpdateCallbackType s);

//cd funcs
DLL_EXPORT void SaturnSetCdGetPhysicalStatusCallback(SaturnCdGetPhysicalStatusCallbackType s);
DLL_EXPORT void SaturnSetCdReadTocCallback(SaturnCdReadTocCallbackType s);
DLL_EXPORT void SaturnSetCdReadSectorAtFadCallback(SaturnCdReadSectorAtFadCallbackType s);

//various ways to physically interact with the saturn
DLL_EXPORT void SaturnPressPowerButton();
DLL_EXPORT void SaturnPressResetButton();

DLL_EXPORT void SaturnOpenCdTray();
DLL_EXPORT void SaturnCloseCdTray();

//run the saturn for one frame, 
//the callbacks you set will be called at the appropriate time during the frame
DLL_EXPORT void SaturnRunFrame();

#ifdef __cplusplus
}
#endif

#endif//SATURNKI_H
