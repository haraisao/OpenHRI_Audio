// -*- C++ -*-
/*!
 * @file  EchoCanceler.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "EchoCanceler.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "EchoCanceler",
    "type_name",         "EchoCanceler",
    "description",       "Echo Canceler",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "component_type",    "STATIC",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
EchoCanceler::EchoCanceler(RTC::Manager* manager)
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
EchoCanceler::~EchoCanceler()
{
}

RTC::ReturnCode_t EchoCanceler::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("EchoCanceler : Echo Canceler"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioDataDataListener("ON_BUFFER_WRITE", this), false); 

  addInPort("ReferenceAudioDataIn", m_ReferenceAudioDataInIn);
  m_ReferenceAudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new ReferenceAudioDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  

  mp_sest = NULL;
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t EchoCanceler::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoCanceler::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t EchoCanceler::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  BufferClr();
  if ( mp_sest == NULL ) {
    int sampleRate = 16000;
    mp_sest = speex_echo_state_init(ECHOLEN, sampleRate * 2);
    speex_echo_ctl(mp_sest, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
  }
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void EchoCanceler::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if (!m_outdata.empty()) {
    m_outdata.clear(); //!< queue buffer clear
  }
  if (!m_indata.empty()) {
    m_indata.clear(); //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void EchoCanceler::RcvInBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if (is_active == true) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));
    unsigned char wk0, wk1;
    short wk;
    int length = data.data.length(); //!< inport data length
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_indata.push_back(wk);
    }
    RTC_DEBUG(("Input1:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_indata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}


void EchoCanceler::RcvOutBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvOutBuffer start"));
  if (is_active == true) {
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
    RTC_DEBUG(("Input2:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_outdata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvOutBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvOutBuffer finish"));
  return;
}


RTC::ReturnCode_t EchoCanceler::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  try {
    is_active = false;
    BufferClr();
    if ( mp_sest != NULL ) {
      speex_echo_state_destroy(mp_sest);
      mp_sest = NULL;
    }
  } catch (...) {
    RTC_WARN(("ec_deinit() failed onDeactivated"));
  }
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoCanceler::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t EchoCanceler::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if((m_indata.size() > BUFFER_MAX) || (m_outdata.size() > BUFFER_MAX)) {
    RTC_INFO(("One of buffers exceeded the maximum value. Start clear buffers."));
    BufferClr();
  }
  if (( m_indata.size() >= ECHOLEN) && (m_outdata.size() >= ECHOLEN)) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int i;
    short *inbuffer = new short[ECHOLEN];
    short *outbuffer = new short[ECHOLEN];
    short *result = new short[ECHOLEN];

    for ( i = 0; i < ECHOLEN; i++ ) {
      inbuffer[i] = m_indata.front();
      m_indata.pop_front();
      outbuffer[i] = m_outdata.front();
      m_outdata.pop_front();
      result[i] = 0;
    }

    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
    speex_echo_cancellation(mp_sest, inbuffer, outbuffer, result);

    delete[] inbuffer;
    delete[] outbuffer;
    m_AudioDataOut.data.length(ECHOLEN * 2);
    for ( i = 0; i < ECHOLEN; i++ ) {
      short val = result[i];
      m_AudioDataOut.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    delete[] result;
    setTimestamp( m_AudioDataOut );
    m_AudioDataOutOut.write();
    RTC_DEBUG(("onExecute:writing %d samples", m_AudioDataOut.data.length() / 2));
  } else {
    RTC_DEBUG(("onExecute:either or both the buffer is under limit"));
  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t EchoCanceler::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t EchoCanceler::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void EchoCancelerInit(RTC::Manager* manager)
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
			     RTC::Create<EchoCanceler>,
			     RTC::Delete<EchoCanceler>);
  }
};
