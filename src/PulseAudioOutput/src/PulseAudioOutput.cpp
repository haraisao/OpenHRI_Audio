// -*- C++ -*-
/*!
 * @file  PulseAudioOutput.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "PulseAudioOutput.h"

#define MIN_INTERNAL_BUFFER 16000

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "PulseAudioOutput",
    "type_name",         "PulseAudioOutput",
    "description",       "Audio Input Component with PlusAudio Library",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "component_type",    "STATIC",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    
    "conf.default.OutputSampleRate", "16000",
    "conf.__type__.OutputSampleRate", "int",
    "conf.default.OutputChannelNumbers", "1",
    "conf.__constraints__.OutputChannelNumbers", "x >= 1",
    "conf.__type__.OutputChannelNumbers", "int",
    "conf.__description__.OutputChannelNumbers", "再生する音声データのチャンネル数",
    "conf.default.OutputSampleByte", "16bits",
    "conf.__constraints__.OutputSampleByte", "8bits,16bits,24bits,32bits",
    "conf.__type__.OutputSampleByte", "string",
    "conf.__description__.OutputSampleByte", "再生する音声のフォーマット(ビット深度)",
    "conf.default.BufferLength", "0.1",
    "conf.__constraints__.BufferLength", "x >= 0",
    "conf.__type__.BufferLength", "double",
    "conf.__description__.BufferLength", "再生する音声データ用のバッファ長（秒）",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PulseAudioOutput::PulseAudioOutput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
    is_active = false;
}

/*!
 * @brief destructor
 */
PulseAudioOutput::~PulseAudioOutput()
{
}

RTC::ReturnCode_t PulseAudioOutput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PulseAudioOutput : Audio Input Component with PlusAudio Library"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("OutputSampleRate", m_OutputSampleRate, "16000");
  bindParameter("OutputChannelNumbers", m_OutputChannelNumbers, "1");
  bindParameter("OutputSampleByte", m_OutputSampleByte, "16bits");
  bindParameter("BufferLength", m_BufferLength, "0.1");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

pa_sample_format PulseAudioOutput::getFormat(std::string str)
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

RTC::ReturnCode_t PulseAudioOutput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  try {
    if ( !m_data.empty() ) {
      RTC_DEBUG(("onFinalize:queue buffer clear start."));
      m_data.clear();  //!< queue buffer clear
      RTC_DEBUG(("onFinalize:queue buffer clear finish."));
    }
    if ( m_simple ) {
      RTC_DEBUG(("onFinalize:simple connection object free start."));
      pa_simple_free( m_simple );
      RTC_DEBUG(("onFinalize:simple connection object free finish."));
      m_simple = NULL;
    }
  } catch (...) {
    RTC_WARN(("pa_simple_free() failed onDFinalize."));
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PulseAudioOutput::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PulseAudioOutput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  try {
    m_spec.format = getFormat(m_OutputSampleByte);
    m_spec.channels = (uint8_t)m_OutputSampleByte;
    m_spec.rate = (uint32_t)m_OutputSampleRate;
    
    m_bufferattr.maxlength = m_bufferattr.minreq = (uint32_t)-1;
    m_bufferattr.tlength = m_bufferattr.prebuf = (uint32_t)(m_BufferLength * m_OutputSampleRate * m_OutputSampleByte);

    m_simple = pa_simple_new(NULL,               //!< Server name, or NULL for default
                             "PulseAudioOutput", //!< A descriptive name for this client (application name, ...)
                             PA_STREAM_PLAYBACK, //!< Open this stream for recording or playback?
                             NULL,               //!< Sink (resp. source) name, or NULL for default
                             "playback",         //!< A descriptive name for this client (application name, song title, ...)
                             &m_spec,            //!< The sample type to use
                             NULL,               //!< The channel map to use, or NULL for default
                             &m_bufferattr,      //!< Buffering attributes, or NULL for default
                             &m_err );           //!< A pointer where the error code is stored when the routine returns NULL. It is OK to pass NULL here.
    
    if ( m_simple == NULL ) {
      throw m_err;
    }

  } catch (...) {
    std::string error_str = pa_strerror(m_err);
    RTC_WARN(("pa_simple_new() failed onActivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }

  if (!m_data.empty()) {
    RTC_DEBUG(("onActivated:queue buffer clear start."));
    m_data.clear();  //!< queue buffer clear
    RTC_DEBUG(("onActivated:queue buffer clear finish."));
  }
  is_active = true;
  m_writezero = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PulseAudioOutput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  try {
    if ( !m_data.empty() ) {
      RTC_DEBUG(("onDeactivated:queue buffer clear start."));
      m_data.clear();  //!< queue buffer clear
      RTC_DEBUG(("onDeactivated:queue buffer clear finish."));
    }

    if ( m_simple ) {
      RTC_DEBUG(("onDeactivated:simple connection object free start."));
      pa_simple_free( m_simple );
      RTC_DEBUG(("onDeactivated:simple connection object free finish."));
      m_simple = NULL;
    }
  } catch (...) {
    RTC_WARN(("pa_simple_free() failed onDeactivated."));
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PulseAudioOutput::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PulseAudioOutput::onExecute(RTC::UniqueId ec_id)
{
  RTC::ReturnCode_t ret = RTC::RTC_OK;
  RTC_DEBUG(("onExecute start"));
  if (m_simple) {
    ret = WriteBuffer();
  }
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}


void PulseAudioOutput::RcvBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( is_active == true ) {
    //! The queuing does input data.
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    RTC_DEBUG(("Queuing input data process start."));
    unsigned long len = data.data.length();  //!< inport data length
    for (unsigned long i = 0; i < len; i++) {
      m_data.push_back((unsigned char)data.data[i]);  //!< enqueue
    }
    RTC_DEBUG(("Queuing input data process finish."));
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  return;
}

RTC::ReturnCode_t PulseAudioOutput::WriteBuffer(void)
{
  // get writable length from audio library
  simple_recast *psimple = (simple_recast *)m_simple;
  pa_threaded_mainloop_lock(psimple->mainloop);
  RTC_DEBUG(("pa_threaded_mainloop_lock()"));

  size_t len = pa_stream_writable_size(psimple->stream);
  if (len == 0) {
    RTC_DEBUG(("WriteBufer: no writable buffer."));
    pa_threaded_mainloop_unlock(psimple->mainloop);
    RTC_DEBUG(("pa_threaded_mainloop_unlock()"));
    return RTC::RTC_OK;
  }

  //  now we copy the data from component buffer to audio device buffer
  m_mutex.lock();
  RTC_DEBUG(("WriteBuffer: mutex lock"));

  RTC_DEBUG(("WriteBuffer: data buffer size: %i", m_data.size()));
  RTC_DEBUG(("WriteBuffer: device writable length: %i", len));
  
  size_t nbytes = len;
  if (nbytes > m_data.size()) nbytes = m_data.size();

  if (m_data.size() == 0) {
    if (len > m_bufferattr.tlength / 2) {
      RTC_DEBUG(("WriteBuffer: data buffer size is zero and device buffer size is low >> write zeros <<"));
      m_writezero = true;
    }
    if (m_writezero == false) {
      RTC_DEBUG(("WriteBufer: no writable data."));
      pa_threaded_mainloop_unlock(psimple->mainloop);
      RTC_DEBUG(("pa_threaded_mainloop_unlock()"));
      m_mutex.unlock();
      RTC_DEBUG(("WriteBuffer: mutex unlock"));
      return RTC::RTC_OK;
    }
  } else {
    m_writezero = false;
  }

  if (m_writezero == true)
      nbytes = len;

  if (nbytes > len)
    nbytes = len;

  // get a data buffer from the audio device
  void *data;
  RTC_DEBUG(("WriteBuffer: prepare to write %i bytes", nbytes));
  if ( pa_stream_begin_write(psimple->stream, &data, &nbytes) < 0 ) {
    RTC_WARN(("pa_stream_begin_write() failed.", nbytes));
    pa_threaded_mainloop_unlock(psimple->mainloop);
    RTC_DEBUG(("pa_threaded_mainloop_unlock()"));
    m_mutex.unlock();
    RTC_DEBUG(("WriteBuffer: mutex unlock"));
    //return RTC::RTC_ERROR;  // this sometimes happen (should retry)
    return RTC::RTC_OK;
  }

  // copy the data to the audio buffer
  RTC_DEBUG(("WriteBuffer: audio buffer dequeue start."));
  unsigned char *buff = (unsigned char *)data;
  if (m_writezero == true) {
    for (unsigned long i = 0; i < (unsigned long)nbytes; i++) {
      *buff++ = 0;
    }
  } else {
    for (unsigned long i = 0; i < (unsigned long)nbytes; i++) {
      *buff++ = m_data.front();
      m_data.pop_front();
    }
  }
  RTC_DEBUG(("WriteBuffer: audio buffer dequeue finish."));

  // set the data to the outport
  m_AudioDataOut.data.length(nbytes);  //!< set outport data length
  memcpy((void *)&(m_AudioDataOut.data[0]), data, nbytes);
  RTC_DEBUG(("WriteBuffer: audio buffer copy complete."));

  m_mutex.unlock();
  RTC_DEBUG(("WriteBuffer: mutex unlock"));

  // send the data to the audio device
  if (pa_stream_write( psimple->stream, data, nbytes, NULL, 0, PA_SEEK_RELATIVE) < 0) {
    RTC_WARN(("pa_stream_write() failed."));
    return RTC::RTC_ERROR;
  }
  pa_threaded_mainloop_unlock(psimple->mainloop);
  RTC_DEBUG(("pa_threaded_mainloop_unlock()"));

  // send the data to the outport
  setTimestamp( m_AudioDataOut );
  m_AudioDataOutOut.write();
  RTC_DEBUG(("wrote stream data to AudioDataOut port"));

  return RTC::RTC_OK;
}
/*
RTC::ReturnCode_t PulseAudioOutput::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PulseAudioOutput::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void PulseAudioOutputInit(RTC::Manager* manager)
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
			     RTC::Create<PulseAudioOutput>,
			     RTC::Delete<PulseAudioOutput>);
  }
};
