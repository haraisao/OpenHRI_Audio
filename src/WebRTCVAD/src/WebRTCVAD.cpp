// -*- C++ -*-
/*!
 * @file  WebRTCVAD.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "WebRTCVAD.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "WebRTCVAD",
    "type_name",         "WebRTCVAD",
    "description",       "VAD using WebRTC",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.FileterLength", "5",
    "conf.__constraints__.FileterLength", "x >= 1",
    "conf.__type__.FileterLength", "int",
    "conf.__description__.FileterLength", "Filter length to smooth voice detection result.",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WebRTCVAD::WebRTCVAD(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AuidoDataInIn("AuidoDataIn", m_AuidoDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WebRTCVAD::~WebRTCVAD()
{
}

RTC::ReturnCode_t WebRTCVAD::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WebRTCVAD : VAD using WebRTC"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AuidoDataIn", m_AuidoDataInIn);
  m_AuidoDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("FileterLength", m_FileterLength, "5");


  WebRtcVad_Create(&handle);
  WebRtcVad_Init(handle);
  WebRtcVad_set_mode(handle, 2); // agressive adaptation mode

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t WebRTCVAD::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WebRTCVAD::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WebRTCVAD::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  m_mutex.lock();

  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  //RTC_INFO(("onActivated start1"));
  if (!m_filterflagbuffer.empty()) {//ADDED Begin
    m_filterflagbuffer.clear();
  }
  //RTC_INFO(("onActivated start2"));
  if (!m_filterdatabuffer.empty()) {
    std::list<WebRtc_Word16*>::iterator it = m_filterdatabuffer.begin();
    while (it != m_filterdatabuffer.end()) {
      delete [] *it;
	  //RTC_INFO(("delete *it"));
	  it++;
	}
    m_filterdatabuffer.clear();
  }//ADDED End

  m_mutex.unlock();

  is_active = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void WebRTCVAD::RcvInBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));

    int length = data.data.length();
    short wk;
    unsigned char wk0, wk1;
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_inbuffer.push_back(wk);
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}


RTC::ReturnCode_t WebRTCVAD::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  //WebRtcVad_Free(handle);
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
*/

/*
RTC::ReturnCode_t WebRTCVAD::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WebRTCVAD::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if (m_inbuffer.size() >= WINLEN) {
    int i;
    WebRtc_Word16 *data;//ADDED
    data = new WebRtc_Word16[WINLEN];//CHANGED
    std::list<short>::iterator pbuffer;

    // sliding window with half overlap
    for (i = 0; i < WINLEN/2; i++) {
      data[i] = m_inbuffer.front();
      m_inbuffer.pop_front();
    }
    pbuffer = m_inbuffer.begin();
    for (i = WINLEN/2; i < WINLEN; i++) {
      data[i] = *pbuffer;
      pbuffer++;
    }

    WebRtc_Word16 vad = WebRtcVad_Process(handle, 16000, data, WINLEN);
    m_filterdatabuffer.push_back(data);//ADDED
    m_filterflagbuffer.push_back(vad);//ADDED
    
    while (m_filterdatabuffer.size() > m_FileterLength) {//CHANGED Begin
      WebRtc_Word16 vad1 = 0;
      std::list<WebRtc_Word16>::iterator it = m_filterflagbuffer.begin();
      for (i = 0; i < m_FileterLength; i++) {
		WebRtc_Word16 vad2 = *it;
		if (vad2 > 0) {
			vad1 = vad2;
		}
		it++;
      }
      //RTC_INFO(("vad: %i, vad(filtered): %i", vad, vad1));

      m_filterflagbuffer.pop_front();
      data = m_filterdatabuffer.front();
      m_filterdatabuffer.pop_front();

      // output the resulting signal
      m_AudioDataOut.data.length(WINLEN);
      if (vad1 > 0) {
		for (i = 0; i < WINLEN/2; i++) {
			m_AudioDataOut.data[i*2] = (unsigned char)(data[i] & 0x00ff);
			m_AudioDataOut.data[i*2+1] = (unsigned char)((data[i] & 0xff00) >> 8);
		}
	  } else {
		for (i = 0; i < WINLEN/2; i++) {
			m_AudioDataOut.data[i*2] = i % 2; // avoid julius zero stripping problem
			m_AudioDataOut.data[i*2+1] = 0;
		}
      }
      delete [] data;
      setTimestamp(m_AudioDataOut);
      Out.write();
    }//CHANGED m_AudioDataOut
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WebRTCVAD::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void WebRTCVADInit(RTC::Manager* manager)
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
			     RTC::Create<WebRTCVAD>,
			     RTC::Delete<WebRTCVAD>);
  }
};
