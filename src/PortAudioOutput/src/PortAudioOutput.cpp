// -*- C++ -*-
/*!
 * @file  PortAudioOutput.cpp
 * @author isao-hara@aist.go.jp
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "PortAudioOutput.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "PortAudioOutput",
    "type_name",         "PortAudioOutput",
    "description",       "Audio Output Component with PortAudio Library",
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
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__type__.OutputSampleRate", "int",
    "conf.__description__.OutputSampleRate", "再生する音声のサンプリング周波数(Hz)",
    "conf.default.OutputChannelNumbers", "1",
    "conf.__constraints__.OutputChannelNumbers", "x >= 1",
    "conf.__type__.OutputChannelNumbers", "int",
    "conf.__description__.OutputChannelNumbers", "再生する音声データのチャンネル数",
    "conf.default.InputSampleByte", "int16",
    "conf.__constraints__.InputSampleByte", "int8,int16,int24,int32",
    "conf.__type__.InputSampleByte", "string",
    "conf.__description__.InputSampleByte", "再生する音声のフォーマット(ビット深度)",
    "conf.default.DelayCount", "0",
    "conf.__constraints__.DelayCount", "x >= 0",
    "conf.__type__.DelayCount", "int",
    "conf.__description__.DelayCount", "再生する音声データ用のバッファ長",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PortAudioOutput::PortAudioOutput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PortAudioOutput::~PortAudioOutput()
{
}

RTC::ReturnCode_t PortAudioOutput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PortAudioOutput : Audio Output Component with PortAudio Library"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new PortAudioOutputDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("OutputSampleRate", m_OutputSampleRate, "16000");
  bindParameter("OutputChannelNumbers", m_OutputChannelNumbers, "1");
  bindParameter("InputSampleByte", m_InputSampleByte, "int16");
  bindParameter("DelayCount", m_DelayCount, "0");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t PortAudioOutput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  try {
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
RTC::ReturnCode_t PortAudioOutput::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/
PaSampleFormat PortAudioOutput::getFormat(std::string str)
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

RTC::ReturnCode_t PortAudioOutput::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated start"));
  PaStreamParameters outputParameters;
  if (Pa_Initialize()  != paNoError ){
    return RTC::RTC_ERROR;
  }
  try {
    m_format = getFormat(m_InputSampleByte);
    m_totalframes = FRAMES_PER_BUFFER * m_OutputChannelNumbers;
    m_err = Pa_GetSampleSize(m_format);
    if ( m_err > 0 ) {
      m_totalframes *= m_err;
    }
    m_data.clear();  //!< queue buffer clear

    int n =  Pa_GetDeviceCount	(	);
    std::cerr << "Device:" << n << std::endl;

    /* stream setiing */
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if ( outputParameters.device < 0 ) {
       std::cerr << "ERROR:" << outputParameters.device << std::endl;
      RTC_ERROR(("No output device"));
      throw (paNotInitialized);
    }

    outputParameters.channelCount = m_OutputChannelNumbers;
    outputParameters.sampleFormat = m_format;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowInputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    m_err = Pa_OpenStream(
          &m_stream,         //!< PortAudioStream
          NULL,              //!< InputParameters
          &outputParameters, //!< outputParameters
          m_OutputSampleRate,      //!< sampleRate
          FRAMES_PER_BUFFER, //!< framesPerBuffer
          paClipOff,         //!< streamFlags:we won't output out of range samples so don't bother clipping
          NULL,              //!< streamCallback:no callback, use blocking API
          NULL );            //!< no callback, so no callback userData

    if ( m_err != paNoError ) {
      throw m_err;
    }

    m_err = Pa_StartStream( m_stream );
    if ( m_err != paNoError ) {
      throw m_err;
    }

  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio failed onActivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }

  if ( !m_data.empty() ) {
    m_data.clear();  //!< queue buffer clear
  }
  is_active = true;
  m_waitflg = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PortAudioOutput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  try {
    if ( !m_data.empty() ) {
      m_data.clear();  //!< queue buffer clear
    }

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
RTC::ReturnCode_t PortAudioOutput::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t PortAudioOutput::onExecute(RTC::UniqueId ec_id)
{
  m_mutex.lock();
  RTC_DEBUG(("WriteBuffer:mutex lock"));
  if (Pa_IsStreamActive(m_stream)) {
    if (FRAMES_PER_BUFFER <= (unsigned long)(Pa_GetStreamWriteAvailable(m_stream))) {
      unsigned char *buffer;  //!< Temporary buffer
      buffer = new unsigned char[m_totalframes];  //!< Temporary buffer

      RTC_DEBUG(("WriteBuffer:audio buffer dequeue start."));
      for ( unsigned long i = 0; i < m_totalframes; i++ ) {
        if ( !m_data.empty() ) {
          buffer[i] = m_data.front();
          m_data.pop_front();  //!< dequeue
        } else {
          buffer[i] = 0;
          m_waitflg = true;
        }
      }

      RTC_DEBUG(("WriteBuffer:audio buffer dequeue finish."));

      m_AudioDataOut.data.length(m_totalframes);  //!< set outport data length
      memcpy((void *)&(m_AudioDataOut.data[0]), buffer, m_totalframes);
      RTC_DEBUG(("WriteBuffer:audio buffer copy complete."));
      setTimestamp( m_AudioDataOut );
      m_AudioDataOutOut.write();
      RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));

      //! write stream
      m_err = Pa_WriteStream( m_stream, buffer, FRAMES_PER_BUFFER );
      if ( m_err != paNoError ) {
        std::string error_str = Pa_GetErrorText(m_err);
        RTC_WARN(("PortAudio WriteStream failed:%s", error_str.c_str()));
        return RTC::RTC_ERROR;
      }
      delete [] buffer;
    }
  } else {
    if ( !m_data.empty() ) {
      m_data.clear();  //!< queue buffer clear
    }
  }
  m_mutex.unlock();
  RTC_DEBUG(("WriteBuffer:mutex unlock"));
  return RTC::RTC_OK;
}

void PortAudioOutput::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( is_active == true ) {
    //! The queuing does input data.
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    unsigned long i = 0;
    if ( m_waitflg == true ) {
      RTC_DEBUG(("Accumulation waiting process start."));
      m_waitflg = false;
      if ( m_delay > 0 ) {
        for ( i = 0; i < (m_delay * m_totalframes); i++ ) {
          m_data.push_back(0);  //!< enqueue
        }
      }
      RTC_DEBUG(("Accumulation waiting process finish."));
    }
    RTC_DEBUG(("Queuing input data process start."));
    unsigned long len = data.data.length();  //!< inport data length
    for ( i = 0; i < len; i++ ) {
      m_data.push_back(data.data[i]);  //!< enqueue
    }
    RTC_DEBUG(("Queuing input data process finish."));
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  return;
}

/*
RTC::ReturnCode_t PortAudioOutput::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t PortAudioOutput::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void PortAudioOutputInit(RTC::Manager* manager)
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
			     RTC::Create<PortAudioOutput>,
			     RTC::Delete<PortAudioOutput>);
  }
};
