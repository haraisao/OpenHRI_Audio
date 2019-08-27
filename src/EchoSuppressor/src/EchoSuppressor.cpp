// -*- C++ -*-
/*!
 * @file  EchoSuppressor.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "EchoSuppressor.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "EchoSuppressor",
    "type_name",         "EchoSuppressor",
    "description",       "Echo Suppressor",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
EchoSuppressor::EchoSuppressor(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_ReferenceAudioDataInIn("ReferenceAudioDataIn", m_ReferenceAudioDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
EchoSuppressor::~EchoSuppressor()
{
}

RTC::ReturnCode_t EchoSuppressor::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("EchoSuppressor : Echo Suppressor"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addInPort("ReferenceAudioDataIn", m_ReferenceAudioDataInIn);
  m_ReferenceAudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new ReferenceAudioDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t EchoSuppressor::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoSuppressor::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t EchoSuppressor::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  BufferClr();
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}


void EchoSuppressor::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if ( !m_indata.empty() ) {
    m_indata.clear();  //!< queue buffer clear
  }
  if ( !m_outdata.empty() ) {
    m_outdata.clear();  //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void EchoSuppressor::RcvInBuffer(RTC::TimedOctetSeq data)
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
      m_indata.push_back(wk);
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

void EchoSuppressor::RcvOutBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvOutBuffer start"));
  if ( is_active == true ) {
    //! The queuing does input data.
    m_mutex.lock();
    RTC_DEBUG(("RcvOutBuffer:mutex lock"));
    unsigned char wk0, wk1;
    short wk;
    int length = data.data.length(); //!< inport data length
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_outdata.push_back(wk);
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvOutBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvOutBuffer finish"));
  return;
}

RTC::ReturnCode_t EchoSuppressor::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoSuppressor::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t EchoSuppressor::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( !m_indata.empty() ) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int num = (int)m_indata.size();
    short *buffer = new short[num];
    short wk;

    for ( int i = 0; i < num; i++ ) {
      buffer[i] = m_indata.front();
      m_indata.pop_front();
      if ( !m_outdata.empty() ) {
        wk = m_outdata.front();
        m_outdata.pop_front();
        if ( wk != 0 ) {
          buffer[i] = 0;
        }
      }
    }
    m_AudioDataOut.data.length(num*2);  //!< set outport data length
    for ( int i = 0; i < num; i++ ) {
      short val = buffer[i];
      m_AudioDataOut.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    delete [] buffer;
    setTimestamp( m_AudioDataOut );
    m_AudioDataOutOut.write();
    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
  } else {
    if ( m_outdata.size() > BUFFER_MAX ) BufferClr();
  }
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoSuppressor::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoSuppressor::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void EchoSuppressorInit(RTC::Manager* manager)
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
			     RTC::Create<EchoSuppressor>,
			     RTC::Delete<EchoSuppressor>);
  }
};
