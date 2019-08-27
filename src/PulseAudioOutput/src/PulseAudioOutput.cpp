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
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
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


RTC::ReturnCode_t PulseAudioOutput::onFinalize()
{

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

  return RTC::RTC_OK;
}

RTC::ReturnCode_t PulseAudioOutput::onDeactivated(RTC::UniqueId ec_id)
{

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
