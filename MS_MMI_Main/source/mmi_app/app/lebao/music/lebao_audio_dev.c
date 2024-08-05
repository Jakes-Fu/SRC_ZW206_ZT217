#include "port_cfg.h"
#include "lebao_audio_dev.h"

#include "std_header.h"
#include "mmk_app.h"
#include "mmi_common.h"
#include "mmiset_export.h"
#include "audio_config.h"
#include "audio_api.h"
#ifdef BLUETOOTH_SUPPORT
#include "mmibt_export.h"
#endif

//-------------------------------audio-------------------------------
// for play file or buffer
// mmisrvaud_api.h
// the priority from high to low is BT -> EARPHONE -> SPEAKER
int lebao_get_audio_srv_route_type(void)
{
	return MMISRVAUD_ROUTE_NONE;
}

// for streaming
// audio_api.h, AUDIO_DEVICE_MODE_TYPE_E
int lebao_get_audio_dev_mode(void)
{
#ifdef BLUETOOTH_SUPPORT
	if (MMIAPIBT_GetActiveBTHeadset())
		return AUDIO_DEVICE_MODE_BLUEPHONE; // AUDIO_DEVICE_MODE_HANDFREE
	else
#endif
	{
		if (MMISRVAUD_IsHeadSetPlugIn())
			return AUDIO_DEVICE_MODE_EARPHONE;
		else
			return AUDIO_DEVICE_MODE_HANDFREE;
	}
}

// for recording
// audio_api.h, AUDIO_DEVICE_MODE_TYPE_E
int lebao_get_record_dev_mode(void)
{
	return AUDIO_DEVICE_MODE_HANDFREE;
}

HAUDIODEV lebao_get_audio_device(void)
{
#ifdef BT_A2DP_SUPPORT		
		return (lebao_get_audio_dev_mode() == AUDIO_DEVICE_MODE_BLUEPHONE) 
		  ? hA2DPDev
      #ifdef PLATFORM_UWS6121E
    		: hAUDDEV;
      #else
        : hARMVB;
      #endif
#else
  #ifdef PLATFORM_UWS6121E
		return hAUDDEV;
  #else
    return hARMVB;
  #endif
#endif
}

HAUDIODEV lebao_get_audio_record_device(void)
{
#ifdef PLATFORM_UWS6121E
  return hAUDDEVRECORD;
#else
  return hARMVBRECORD;
#endif
}

void lebao_open_bt_device(void)
{
#ifdef BLUETOOTH_SUPPORT
  if (MMIAPIBT_GetActiveBTHeadset())
	  MMIBT_OpenDevice(0); // MMISRVAUD_SAMPLE_RATE_DEFAULT
#endif
}

void lebao_set_headset_default_volume(void)
{
// overwrite the last setting
// reduce the default volume when the headset is connected
#ifdef BLUETOOTH_SUPPORT
	if (MMIAPIBT_GetActiveBTHeadset())
		lebao_set_volume(MMISET_VOL_FIVE); 
	else
#endif
	{
		if (MMISRVAUD_IsHeadSetPlugIn())
			lebao_set_volume(MMISET_VOL_FIVE);
		else
			lebao_set_volume(MMISET_VOL_MAX);
	}
}


//-----------------------------set the volume----------------------------------
static int _maxVolume = MMISET_VOL_MAX;
static int _minVolume = MMISET_VOL_ONE;
static int _currentVolume = MMISET_VOL_NINE;

void lebao_set_max_volume(const int vol)
{
    _maxVolume = (vol <= 0) ? _minVolume : vol;
}

int lebao_get_max_volume(void)
{
    return (_maxVolume <= 0) ? _minVolume : _maxVolume;
}

int lebao_set_volume(const int vol)
{
	int realVol = vol;

	if (realVol < _minVolume) realVol = _minVolume;
	else if (realVol > _maxVolume) realVol = _maxVolume;

	_currentVolume = realVol;
	if (lebao_get_audioHandle() != 0) {
		if (lebao_get_playType() == 0 || lebao_get_playType() == 3)
			AUDIO_SetVolume(_currentVolume);
		else
			MMISRVAUD_SetVolume(lebao_get_audioHandle(), _currentVolume);
	}	
	return 0;
}

void lebao_set_default_volume(void)
{
	lebao_set_volume(_currentVolume);
}

// real volume
int lebao_get_volume(void)
{
	return _currentVolume;
}

int lebao_get_volume_percent(void)
{
	int per = (_currentVolume * 100) / _maxVolume;

	if (per > 100) per = 100;
	else if (per < 10) per = 10;

	return per;
}

int lebao_add_volume(void)
{
    ++_currentVolume;
    if (_currentVolume < _minVolume) _currentVolume = _minVolume;
    else if (_currentVolume > _maxVolume) _currentVolume = _maxVolume;

    return _currentVolume;
}

int lebao_sub_volume(void)
{
    --_currentVolume;
    if (_currentVolume < _minVolume) _currentVolume = _minVolume;
    else if (_currentVolume > _maxVolume) _currentVolume = _maxVolume;

    return _currentVolume;
}

int lebao_get_volume_step(void)
{
	return 1;
}

