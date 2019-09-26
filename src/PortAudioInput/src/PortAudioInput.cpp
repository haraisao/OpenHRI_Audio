// -*- C++ -*-
/*!
 * @file  PortAudioInput.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "PortAudioInput.h"

#if defined(_WIN32)
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0601
#endif
#define PA_USE_WASAPI
#endif

#ifdef HAVE_LIBPORTMIXER
// use portmixer"s internal volume control functions
extern "C" {
  void *Px_OpenMixer( void *pa_stream, int i );
  void Px_CloseMixer(void *mixer);
  float Px_GetInputVolume( void *mixer );
  void Px_SetInputVolume( void *mixer, float volume );
}
#endif

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "PortAudioInput",
    "type_name",         "PortAudioInput",
    "description",       "Audio Input Component with PortAudio Library",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "component_type",    "STATIC",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    
    "conf.default.InputSampleRate", "16000",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__type__.InputSampleRate", "int",
    "conf.__description__.InputSampleRate", "録音する音声のサンプリング周波数(Hz)",
    "conf.default.InputChannelNumbers", "1",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__type__.InputChannelNumbers", "int",
    "conf.__description__.InputChannelNumbers", "録音する音声データのチャンネル数",
    "conf.default.InputSampleByte", "int16",
    "conf.__constraints__.InputSampleByte", "(int8,int16,int24,int32)",
    "conf.__type__.InputSampleByte", "string",
    "conf.__description__.InputSampleByte", "録音する音声のフォーマット(ビット深度)",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PortAudioInput::PortAudioInput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_GainDataInIn("GainDataIn", m_GainDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PortAudioInput::~PortAudioInput()
{
}

RTC::ReturnCode_t PortAudioInput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PortAudioInput : Audio Input Component with PortAudio Library"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("GainDataIn", m_GainDataInIn);
  m_GainDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new PortAudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("InputSampleRate", m_InputSampleRate, "16000");
  bindParameter("InputChannelNumbers", m_InputChannelNumbers, "1");
  bindParameter("InputSampleByte", m_InputSampleByte, "int16");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


PaSampleFormat PortAudioInput::getFormat(std::string str)
{
  if ( str == "int8" ) {
    return paInt8;
  } else if ( str == "int16" ) {
    return paInt16;
  } else if ( str == "int24" ) {
    return paInt24;
  } else if ( str == "int32" ) {
    return paInt32;
  } else {
    return paInt16;
  }
}

#if defined(_WIN32)
bool PortAudioInput::InitMixer(void)
{
  MIXERLINE         mxl;
  MIXERCONTROL      mxc;
  MIXERLINECONTROLS mxlc;
  MMRESULT          mmresult;
  DWORD dwLineID = -1;

  m_isOpen = false;
  m_mixerNums = mixerGetNumDevs();
  if ( m_mixerNums < 1 ) return FALSE;
  m_mixerNums = min(m_mixerNums, HMIXER_NUM_MAX);

  mxl.cbStruct = sizeof(MIXERLINE);
  mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

  /* Mixer Init */
  for ( int i = 0; i < m_mixerNums; i++ ) {
    mmresult = mixerOpen( &m_mixer[i], (UINT)i, 0, 0, MIXER_OBJECTF_MIXER );
    if ( mmresult == MMSYSERR_NOERROR ) {
      m_barrOpened[i] = true;
      m_isOpen = true;
      mmresult = mixerGetLineInfo( (HMIXEROBJ)m_mixer[i], &mxl,
				   MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
      if ( mmresult == MMSYSERR_NOERROR ) {
        m_mixerMic = m_mixer[i];
        break;
      }
    }
  }
  if ( m_mixerMic == 0 ) return false;
  /* Volume Control Init */
  for ( DWORD i = 0; i < mxl.cConnections; i++ ) {
    mxl.dwSource = i;
    mmresult = mixerGetLineInfo( (HMIXEROBJ)m_mixerMic, &mxl,
				 MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);
    if ( mmresult != MMSYSERR_NOERROR ) {
      break;
    }
    if ( mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE ) {
      dwLineID = mxl.dwLineID;
      break;
    }
  }
  if ( dwLineID == -1 ) {
    return false;
  }

  mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
  mxlc.dwLineID = dwLineID;
  mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
  mxlc.cControls = 1;
  mxlc.cbmxctrl = sizeof(MIXERCONTROL);
  mxlc.pamxctrl = &mxc;
  mmresult = mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				  &mxlc,
				  MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
  if ( mmresult != MMSYSERR_NOERROR ) return false;
  m_dwCntlID = mxc.dwControlID;
  m_dwMax = mxc.Bounds.dwMaximum;
  m_dwMin = mxc.Bounds.dwMinimum;
  return true;
}

void PortAudioInput::CloseMixer(void)
{
  m_dwCntlID = 0;
  if ( m_isOpen ) {
    for (int i = 0; i < m_mixerNums; i++ ) {
      if ( m_barrOpened[i] ) {
        mixerClose(m_mixer[i]);
        m_barrOpened[i] = false;
      }
    }
    m_isOpen = false;
  }
}

bool PortAudioInput::GetMicrophoneLevel(DWORD* dwLevel)
{
  MMRESULT mmresult;
  MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
  MIXERCONTROLDETAILS   mxcd;

  mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
  mxcd.dwControlID = m_dwCntlID;
  mxcd.cChannels = 1;
  mxcd.cMultipleItems = 0;
  mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
  mxcd.paDetails = &mxcdVolume;
  mmresult = mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				    &mxcd,
				    MIXER_GETCONTROLDETAILSF_VALUE);
  if (mmresult != MMSYSERR_NOERROR) return false;
  *dwLevel = mxcdVolume.dwValue;
  return true;
}

bool PortAudioInput::SetMicrophoneLevel(DWORD dwLevel)
{
  MMRESULT mmresult;
  MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set = {
    dwLevel
  };
  MIXERCONTROLDETAILS mxcd_Set;
  mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);
  mxcd_Set.dwControlID = m_dwCntlID;
  mxcd_Set.cChannels = 1;
  mxcd_Set.cMultipleItems = 0;
  mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
  mxcd_Set.paDetails = &mxcdVolume_Set;
  mmresult = mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				    &mxcd_Set,
				    MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);
  return ( mmresult == MMSYSERR_NOERROR );
}
#elif defined(__linux)

#endif

RTC::ReturnCode_t PortAudioInput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  try {
    m_AudioDataOutOut.deactivateInterfaces();
    if ( m_stream ) {
      if ( !Pa_IsStreamStopped( m_stream ) ) {
        m_err = Pa_AbortStream( m_stream );
        if ( m_err != paNoError ) {
          throw m_err;
        }
      }

      m_err = Pa_CloseStream( m_stream );
      if ( m_err != paNoError ) {
        throw m_err;
      }
      m_stream = NULL;
    }
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio Stream close failed onFinalize:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PortAudioInput::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PortAudioInput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  PaStreamParameters inputParameters;
  if (Pa_Initialize()  != paNoError ){
    return RTC::RTC_ERROR;
  }
  try {
    m_format = getFormat(m_InputSampleByte);
    m_totalframes = FRAMES_PER_BUFFER * m_InputChannelNumbers;
    m_err = Pa_GetSampleSize(m_format);
    if( m_err > 0 ) {
      m_totalframes *= m_err;
    }

    inputParameters.device = Pa_GetDefaultInputDevice(); //!< default input device
    if ( inputParameters.device < 0 ) {
      throw (paNotInitialized);
    }
    if ( m_InputChannelNumbers > Pa_GetDeviceInfo(inputParameters.device)->maxInputChannels )
      m_InputChannelNumbers = Pa_GetDeviceInfo(inputParameters.device)->maxInputChannels;
    inputParameters.channelCount = m_InputChannelNumbers;
    inputParameters.sampleFormat = m_format;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;

    inputParameters.hostApiSpecificStreamInfo = NULL;
    m_err = Pa_OpenStream(
          &m_stream,         //!< PortAudioStream
          &inputParameters,  //!< InputParameters
          NULL,              //!< outputParameters
          m_InputSampleRate,      //!< sampleRate
          FRAMES_PER_BUFFER, //!< framesPerBuffer
          paClipOff,         //!< streamFlags:we won't output out of range samples so don't bother clipping
          NULL,              //!< streamCallback:no callback, use blocking API
          NULL );            //!< no callback, so no callback userData
    if( m_err != paNoError ) {
      throw m_err;
    }
#ifdef HAVE_LIBPORTMIXER
    m_mixer = Px_OpenMixer( m_stream, 0 );
    m_volume = Px_GetInputVolume( m_mixer );
#else
#  if defined(_WIN32)
    if ( InitMixer() == true ) {
      DWORD vol;
      GetMicrophoneLevel(&vol);
    } else {
      CloseMixer();
    }
#  elif defined(__linux)
    const char* sound_device_names[] = SOUND_DEVICE_NAMES;
    m_device = -1;
    m_fd = -1;
    for ( int i = 0; i < SOUND_MIXER_NRDEVICES; i ++ ) {
      std::cout << " device name : " << sound_device_names[i] << std::endl;
      if ( strcmp( "mic", sound_device_names[i] ) == 0 ) {
        m_device = i;
        break;
      }
    }
    if ( ( m_fd = open( "/dev/mixer", O_RDONLY ) ) == -1 ) {
      perror( "open" );
    }
#  endif
#endif

    m_err = Pa_StartStream( m_stream );
    if( m_err != paNoError ) {
      throw m_err;
    }

  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio failed onActivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }

  syncflg = true;
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}


void PortAudioInput::SetGain(long m_gain)
{
#ifdef USE_PORTMIXER
  Px_SetInputVolume( m_mixer, float(m_volume * m_gain) );
#else
#if defined(_WIN32)
  if ( m_mixerMic != NULL ) {
    DWORD vol;
    GetMicrophoneLevel(&vol);
    vol = vol * m_gain;
    SetMicrophoneLevel(vol);
  }
#elif defined(__linux)
  int level = (int)m_gain;
  if ( ( m_fd > 0 ) && ( m_device > 0 ) ) {
    if ( ioctl( m_fd, MIXER_WRITE( m_device ), &level ) == -1 ) {
      perror( "ioctl(MIXER_WRITE())" );
    }
  }
#endif
#endif
  return;
}


RTC::ReturnCode_t PortAudioInput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  try {
    if ( m_stream ) {
      if ( !Pa_IsStreamStopped( m_stream ) ) {
        m_err = Pa_AbortStream( m_stream );
        if ( m_err != paNoError ) {
          throw m_err;
        }
      }
#ifdef HAVE_LIBPORTMIXER
      Px_CloseMixer(m_mixer);
#elif defined(__linux)
      if ( m_fd > 0 ) {
        close( m_fd );
        m_fd = -1;
      }
#endif

      m_err = Pa_CloseStream( m_stream );
      if ( m_err != paNoError ) {
          throw m_err;
      }
      m_stream = NULL;
    }
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio Stream close failed onDeactivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }
  m_mutex.unlock();
  Pa_Terminate(); 
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PortAudioInput::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PortAudioInput::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  //! The data read from the device is output.
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if ( Pa_IsStreamActive(m_stream) ) {
    m_AudioDataOut.data.length(m_totalframes); //!< set outport data length
    m_err = Pa_ReadStream( m_stream, (void *)&(m_AudioDataOut.data[0]), FRAMES_PER_BUFFER );

    if ( m_err != paNoError ) {
      std::string error_str = Pa_GetErrorText(m_err);
      RTC_WARN(("PortAudio ReadStream failed:%s", error_str.c_str()));
    }
    setTimestamp(m_AudioDataOut);
    m_AudioDataOutOut.write();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PortAudioInput::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioInput::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void PortAudioInputInit(RTC::Manager* manager)
  {
    int i, j;

    for (i = 0; strlen(rtcomponent_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl[j] = (char *)rtcomponent_spec[j];
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
			     RTC::Create<PortAudioInput>,
			     RTC::Delete<PortAudioInput>);
  }
};
