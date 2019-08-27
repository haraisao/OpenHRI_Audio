// -*- C++ -*-
/*!
 * @file  PulseAudioInput.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "PulseAudioInput.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "PulseAudioInput",
    "type_name",         "PulseAudioInput",
    "description",       "Audio Input Component with PulseAudio Library",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.InputSampleRate", "16000",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__type__.InputSampleRate", "int",
    "conf.__description__.InputSampleRate", "録音する音声のサンプリング周波数(Hz)",
    "conf.default.InputChannelNumbers", "1",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__type__.InputChannelNumbers", "int",
    "conf.__description__.InputChannelNumbers", "録音する音声データのチャンネル数",
    "conf.default.InputSampleByte", "16bits",
    "conf.__constraints__.InputSampleByte", "8bits,16bits,24bits,32bits",
    "conf.__type__.InputSampleByte", "string",
    "conf.__description__.InputSampleByte", "録音する音声のフォーマット(ビット深度)",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PulseAudioInput::PulseAudioInput(RTC::Manager* manager)
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
PulseAudioInput::~PulseAudioInput()
{
}

RTC::ReturnCode_t PulseAudioInput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PulseAudioInput : Audio Input Component with PulseAudio Library"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("GainDataIn", m_GainDataInIn);
  m_GainDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new GainDataDataListener("ON_BUFFER_WRITE", this), false); 

  m_AudioDataOut.tm.nsec = 0;
  m_AudioDataOut.tm.sec = 0;
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("InputSampleRate", m_spec.rate, "16000");
  bindParameter("InputChannelNumbers", m_InputChannelNumbers, "1");
  bindParameter("InputSampleByte", m_InputSampleByte, "16bits");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

pa_sample_format PulseAudioInput::getFormat(std::string str)
{
  if ( str == "int8" ) {
    return PA_SAMPLE_U8;
  } else if ( str == "int16" ) {
    return PA_SAMPLE_S16LE;
  } else if ( str == "int24" ) {
    return PA_SAMPLE_S24LE ;
  } else if ( str == "int32" ) {
    return PA_SAMPLE_S32LE ;
  } else {
    return PA_SAMPLE_S16LE;
  }
}


RTC::ReturnCode_t PulseAudioInput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( m_simple ) {
    RTC_DEBUG(("onFinalize:simple connection object free start."));
    pa_simple_free( m_simple );
    RTC_DEBUG(("onFinalize:simple connection object free finish."));
   m_simple = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PulseAudioInput::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PulseAudioInput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  try {
    pa_cvolume cv;
//    mp_vol = pa_cvolume_reset(&cv, 1);
//    pa_cvolume_init(mp_vol);
    m_spec.format = getFormat(m_InputSampleByte);
    m_spec.channels = (uint8_t)m_InputChannelNumbers;

    m_simple = pa_simple_new(
                  NULL,               //!< Server name, or NULL for default
                  "PulseAudioInput",  //!< A descriptive name for this client (application name, ...)
                  PA_STREAM_RECORD,   //!< Open this stream for recording or playback?
                  NULL,               //!< Sink (resp. source) name, or NULL for default
                  "record",           //!< A descriptive name for this client (application name, song title, ...)
                  &m_spec,            //!< The sample type to use
                  NULL,               //!< The channel map to use, or NULL for default
                  NULL,               //!< Buffering attributes, or NULL for default
                  &m_err );           //!< A pointer where the error code is stored when the routine returns NULL. It is OK to pass NULL here.
    if ( m_simple == NULL ) {
      throw m_err;
    }
  } catch (...) {
    std::string error_str = pa_strerror(m_err);
    RTC_WARN(("pa_simple_new() failed onActivated:%s", error_str.c_str()));
  }
  is_active = true;

  RTC_DEBUG(("onActivated finish"));

  return RTC::RTC_OK;
}

void PulseAudioInput::SetGain(long m_gain)
{
  pa_cvolume v;
  v.values[0] = pa_sw_volume_from_linear(m_gain);
  pa_cvolume_set(&v, 2, v.values[0]);
  //      pa_volume_t gain = pa_sw_volume_from_linear(m_gain);
  //      pa_cvolume_set(mp_vol, 1, gain);
  return;
}


RTC::ReturnCode_t PulseAudioInput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( m_simple ) {
    RTC_DEBUG(("onDeactivated:simple connection object free start."));
    pa_simple_free( m_simple );
    RTC_DEBUG(("onDeactivated:simple connection object free finish."));
    m_simple = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PulseAudioInput::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PulseAudioInput::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if( m_simple ) {
    int r;
    simple_recast *psimple = (simple_recast *)m_simple;
    pa_threaded_mainloop_lock( psimple->mainloop );
    RTC_DEBUG(("pa_threaded_mainloop_lock()"));

    while ( !psimple->read_data ) {
      r = pa_stream_peek( psimple->stream, &psimple->read_data, &psimple->read_length );
      if ( !psimple->read_data ) {
        RTC_DEBUG(("pa_stream_peek():no readable data. wait start."));
        pa_threaded_mainloop_wait(psimple->mainloop);
      }
    }

    m_AudioDataOut.data.length( psimple->read_length );  //!< set outport data length
    memcpy((void *)&(m_AudioDataOut.data[0]), (const uint8_t*) psimple->read_data, psimple->read_length);

    r = pa_stream_drop( psimple->stream );
    if ( r < 0 ) {
      RTC_WARN(("pa_stream_drop():capture stream drop failed."));
    }
    psimple->read_data = NULL;
    psimple->read_length = 0;
    psimple->read_index = 0;

    setTimestamp( m_AudioDataOut );
    m_AudioDataOutOut.write();
    RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));

    pa_threaded_mainloop_unlock( psimple->mainloop );
    RTC_DEBUG(("pa_threaded_mainloop_unlock()"));
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PulseAudioInput::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioInput::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void PulseAudioInputInit(RTC::Manager* manager)
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
			     RTC::Create<PulseAudioInput>,
			     RTC::Delete<PulseAudioInput>);
  }
};
