// -*- C++ -*-
/*!
 * @file  SignalGeneration.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "SignalGeneration.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "SignalGeneration",
    "type_name",         "SignalGeneration",
    "description",       "Signal Generator",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.Mode", "Square",
    "conf.__constraints__.Mode", "Square,Triangle, Sin",
    "conf.__type__.Mode", "string",
    "conf.__description__.Mode", "生成する信号を設定（矩形波、三角波、正弦波）",
    "conf.default.Frequency", "100",
    "conf.__constraints__.Frequency", "x >= 1",
    "conf.__type__.Frequency", "int",
    "conf.__description__.Frequency", "生成する信号の周波数(Hz)",
    "conf.default.Gain", "1000",
    "conf.__constraints__.Gain", "x >= 1",
    "conf.__type__.Gain", "int",
    "conf.__description__.Gain", "生成する信号の振幅",
    "conf.default.OutputSampleRate", "16000",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__type__.OutputSampleRate", "int",
    "conf.__description__.OutputSampleRate", "出力する信号データのサンプリング周波数(Hz)",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
SignalGeneration::SignalGeneration(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
SignalGeneration::~SignalGeneration()
{
}

RTC::ReturnCode_t SignalGeneration::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("SignalGeneration : Signal Generator"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("Mode", m_Mode, "Square");
  bindParameter("Frequency", m_Frequency, "100");
  bindParameter("Gain", m_Gain, "1000");
  bindParameter("OutputSampleRate", m_OutputSampleRate, "16000");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t SignalGeneration::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SignalGeneration::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t SignalGeneration::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  try {
    m_pbuff = 0;
    m_flg = true;
    m_cnt = 0;
    m_timer = coil::gettimeofday() - 1.0;
  } catch (...) {
    RTC_WARN(("%s", "error onActivated."));
    return RTC::RTC_ERROR;
  }

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SignalGeneration::onDeactivated(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SignalGeneration::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t SignalGeneration::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  coil::TimeValue now = coil::gettimeofday();
  long bufferlen = long((now - m_timer) * m_OutputSampleRate);
  if ( bufferlen <= 0 ) return RTC::RTC_OK;
  m_timer = now;
  int len = (int)( m_OutputSampleRate / m_Frequency );
  short *buffer = new short[bufferlen];
  double d = 0;
  if ( m_Mode == "Triangle" ) {
    d = m_Gain / (len / 4);
  } else if ( m_Mode == "Sin" ) {
    d = M_PI * 2 / len;
  }
  for ( int i = 0; i < bufferlen; i++) {
    if ( m_Mode == "Square" ) {
      if ( m_cnt < len / 2 ) {
        m_pbuff = m_Gain;
      } else {
        m_pbuff = ~m_Gain;
      }
    } else if ( m_Mode == "Triangle" ) {
      if ( m_flg == true ) {
        m_pbuff += d;
        if (m_pbuff > m_Gain) {
          m_flg = false;
          m_pbuff = m_pbuff - (d * 2);
        }
      } else {
        m_pbuff -= d;
        if (m_pbuff < ~m_Gain) {
          m_flg = true;
          m_pbuff = m_pbuff + (d * 2);
        }
      }
    } else if ( m_Mode == "Sin" ) {
      m_pbuff = m_Gain * sin( d * m_cnt );
    } else {
      m_pbuff = 0;
    }
    m_cnt++;
    if ( m_cnt >= len ) {
      m_cnt = 0;
      if ( m_Mode != "Square" ) m_pbuff = 0;
    }
    buffer[i] = (short)m_pbuff;
  }
  m_AudioDataOut.data.length(bufferlen * 2);  //!< set outport data length
  memcpy((void *)&(m_AudioDataOut.data[0]), (void *)&(buffer[0]), bufferlen * 2);
  setTimestamp(m_AudioDataOut);
  m_AudioDataOutOut.write();
  delete [] buffer;

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SignalGeneration::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SignalGeneration::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void SignalGenerationInit(RTC::Manager* manager)
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
			     RTC::Create<SignalGeneration>,
			     RTC::Delete<SignalGeneration>);
  }
};
