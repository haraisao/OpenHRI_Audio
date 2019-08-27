// -*- C++ -*-
/*!
 * @file  CombFilter.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "CombFilter.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "CombFilter",
    "type_name",         "CombFilter",
    "description",       "Comb Filter",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.SampleRate", "16000",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__type__.SampleRate", "int",
    "conf.__description__.SampleRate", "入力音声データのサンプリング周波数 (Hz)",
    "conf.default.Frequency", "100",
    "conf.__constraints__.Frequency", "x >= 0",
    "conf.__type__.Frequency", "int",
    "conf.__description__.Frequency", "ノイズを削減する周波数 (Hz)",
    "conf.default.Gain", "-1",
    "conf.__constraints__.Gain", "-1 <= x <= 1",
    "conf.__type__.Gain", "int",
    "conf.__description__.Gain", "加減算する度合い",
    "conf.default.ChannelNumbers", "1",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__type__.ChannelNumbers", "int",
    "conf.__description__.ChannelNumbers", "出力する音声データのチャンネル数",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CombFilter::CombFilter(RTC::Manager* manager)
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
CombFilter::~CombFilter()
{
}

RTC::ReturnCode_t CombFilter::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("CombFilter : Comb Filter"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioDataDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("SampleRate", m_SampleRate, "16000");
  bindParameter("Frequency", m_Frequency, "100");
  bindParameter("Gain", m_Gain, "-1");
  bindParameter("ChannelNumbers", m_ChannelNumbers, "1");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t CombFilter::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CombFilter::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t CombFilter::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( !m_dbuff.empty() ) {
    m_dbuff.clear();
  }

  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void CombFilter::RcvInBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));
    //! The queuing does input data.
    unsigned char wk0, wk1;
    short wk;
    int length = data.data.length(); //!< inport data length
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_queue.push_back(wk);  //!< enqueue
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

RTC::ReturnCode_t CombFilter::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CombFilter::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t CombFilter::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  int delay = (int)( m_SampleRate / m_Frequency );
  int num = m_queue.size();
  if ( num > delay ) {
    int i,j;
    short *buffer = new short[num];
    short *dbuffer = new short[num];
    for ( j = 0; j < delay; j++ ) {
      if( !m_dbuff.empty() ) {
        dbuffer[j] = m_dbuff.front();
        m_dbuff.pop_front();  //!< dequeue
      } else {
        dbuffer[j] = 0;
       }
    }
    for ( i = 0; i < num; i++ ) {
      //! takes buffer
      buffer[i] = m_queue.front();
      m_queue.pop_front();  //!< dequeue
      if( j < num ) {
        dbuffer[j] = buffer[i];
      } else {
        m_dbuff.push_back(buffer[i]);
      }
      j++;
    }
    for ( i = 0; i < num; i++ ) {
      buffer[i] += dbuffer[i] * m_Gain;
    }
    //! output the added forequarter.
    m_AudioDataOut.data.length(num*2);  //!< set outport data length
    for ( i = 0; i < num; i++ ) {
      short val = buffer[i];
      m_AudioDataOut.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    setTimestamp( m_AudioDataOut );
    m_AudioDataOutOut.write();
    delete[] buffer;
    delete[] dbuffer;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CombFilter::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CombFilter::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void CombFilterInit(RTC::Manager* manager)
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
			     RTC::Create<CombFilter>,
			     RTC::Delete<CombFilter>);
  }
};
