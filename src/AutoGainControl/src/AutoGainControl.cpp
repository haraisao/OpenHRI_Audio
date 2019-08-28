// -*- C++ -*-
/*!
 * @file  AutoGainControl.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "AutoGainControl.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "AutoGainControl",
    "type_name",         "AutoGainControl",
    "description",       "Auto Gain Control",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.GainCtlSampleByte", "int16",
    "conf.__constraints__.GainCtlSampleByte", "int8,int16,int24,int32",
    "conf.__type__.GainCtlSampleByte", "string",
    "conf.__description__.GainCtlSampleByte", "ゲイン調整のバイト数",
    "conf.default.InputSampleByte", "int16",
    "conf.__constraints__.InputSampleByte", "int8,int16,int24,int32",
    "conf.__type__.InputSampleByte", "string",
    "conf.__description__.InputSampleByte", "録音する音声のフォーマット設定",
    "conf.default.AutomaticGainControl", "100",
    "conf.__constraints__.AutomaticGainControl", "x >= 0",
    "conf.__type__.AutomaticGainControl", "int",
    "conf.__description__.AutomaticGainControl", "ゲイン調整パラメータ",
    "conf.default.InputChannelNumbers", "int",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__type__.InputChannelNumbers", "1",
    "conf.__description__.InputChannelNumbers", "出力する音声データのチャンネル数設定",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
AutoGainControl::AutoGainControl(RTC::Manager* manager)
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
AutoGainControl::~AutoGainControl()
{
}

RTC::ReturnCode_t AutoGainControl::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("AutoGainControl : Auto Gain Control"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioDataInDataListener("ON_BUFFER_WRITE", this), false); 

  m_AudioDataOut.tm.nsec = 0;
  m_AudioDataOut.tm.sec = 0;
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("GainCtlSampleByte", m_GainCtlSampleByte, "int16");
  bindParameter("InputSampleByte", m_InputSampleByte, "int16");
  bindParameter("AutomaticGainControl", m_AutomaticGainControl, "100");
  bindParameter("InputChannelNumbers", m_InputChannelNumbers, "1");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t AutoGainControl::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t AutoGainControl::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t AutoGainControl::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  m_gain = 1;
  m_div1 = m_div2 = 0;
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void AutoGainControl::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( ( is_active == true ) && ( m_agc != 0) ) {
    //! The queuing does input data.
    unsigned char val[4];
    int length = (int)data.data.length();
    int i;
    double sum = 0;
    
    if ( m_GainCtlSampleByte == "int8" ) {
      RTC_DEBUG(("int8 input data process start."));
      for ( i = 0; i < length; i++ ) {
        val[0] = (unsigned char)data.data[i];
        sum += pow( (double)val[0], 2.0);
      }
      RTC_DEBUG(("int8 input data process finish."));
      
    } else if ( m_GainCtlSampleByte == "int16" ) {
      RTC_DEBUG(("int16 input data process start."));
      for ( i = 0; i < length/2; i++ ) {
        val[0] = (unsigned char)data.data[i*2];
        val[1] = (unsigned char)data.data[i*2+1];
        short wk = (short)val[0] + (short)(val[1] << 8);
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int16 input data process finish."));
      
    } else if ( m_GainCtlSampleByte == "int24" ) {
      RTC_DEBUG(("int24 input data process start."));
      for ( i = 0; i < length/3; i++ ) {
        val[0] = (unsigned char)data.data[i*3];
        val[1] = (unsigned char)data.data[i*3+1];
        val[2] = (unsigned char)data.data[i*3+2];
        long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
        if ( wk > 8388607 ) {
          wk = wk - 16777215;
        }
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int24 input data process finish."));
      
    } else if ( m_GainCtlSampleByte == "int32" ) {
      RTC_DEBUG(("int24 input data process start."));
      for ( i = 0; i < length/4; i++ ) {
        val[0] = (unsigned char)data.data[i*4];
        val[1] = (unsigned char)data.data[i*4+1];
        val[2] = (unsigned char)data.data[i*4+2];
        val[3] = (unsigned char)data.data[i*4+3];
        long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int24 input data process finish."));
      
    } else {
      sum = 0;
    }
    sum = sqrt( sum / length );
    sum = sum - m_agc;
    double d = 0.0001 * ( sum - m_div1 );
    m_gain = m_gain + d;
    m_div2 = m_div1;
    m_div1 = sum;
  }
  return;
}

RTC::ReturnCode_t AutoGainControl::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t AutoGainControl::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t AutoGainControl::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  m_AudioDataOut.data = (int)m_gain;
  setTimestamp( m_AudioDataOut );
  m_AudioDataOutOut.write();
  RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t AutoGainControl::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t AutoGainControl::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void AutoGainControlInit(RTC::Manager* manager)
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
			     RTC::Create<AutoGainControl>,
			     RTC::Delete<AutoGainControl>);
  }
};
