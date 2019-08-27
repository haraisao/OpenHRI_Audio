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
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("InputSampleRate", m_InputSampleRate, "16000");
  bindParameter("InputChannelNumbers", m_InputChannelNumbers, "1");
  bindParameter("InputSampleByte", m_InputSampleByte, "16bits");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t PulseAudioInput::onFinalize()
{

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

  return RTC::RTC_OK;
}

RTC::ReturnCode_t PulseAudioInput::onDeactivated(RTC::UniqueId ec_id)
{

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
