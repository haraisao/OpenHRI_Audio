// -*- C++ -*-
/*!
 * @file  Mixer.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "Mixer.h"


#include <coil/stringutil.h>

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v)
{
  std::string s;
  std::vector<std::string> sv;
  is >> s;
  sv = coil::split(s ,",");
  v.resize(sv.size());
  for (int i(0), len(sv.size()); i < len; ++i) {
    T tv;
    if (coil::stringTo(tv, sv[i].c_str())) {
      v[i] = tv;
    }
  }
  return is;
}

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "Mixer",
    "type_name",         "Mixer",
    "description",       "Mixer",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.OutputChannelNumbers", "1",
    "conf.__constraints__.OutputChannelNumbers", "x >= 1",
    "conf.__type__.OutputChannelNumbers", "int",
    "conf.__description__.OutputChannelNumbers", "出力する音声データのチャンネル数",
    "conf.default.MixGains", "0.5,0.5",
    "conf.__type__.MixGains", "std::vector<double>",
    "conf.__description__.MixGains", "各チャンネルの混合重み",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Mixer::Mixer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_micAudioDataInIn("micAudioDataIn", m_micAudioDataIn),
    m_micReferenceAudioDataInIn("micReferenceAudioDataIn", m_micReferenceAudioDataIn),
    m_micAudioDataOutOut("micAudioDataOut", m_micAudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Mixer::~Mixer()
{
}

RTC::ReturnCode_t Mixer::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("Mixer : Mixer"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("micAudioDataIn", m_micAudioDataInIn);
  m_micAudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new MicAudioDataListener("ON_BUFFER_WRITE", this), false); 

  addInPort("micReferenceAudioDataIn", m_micReferenceAudioDataInIn);
  m_micReferenceAudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new MicReferenceDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("micAudioDataOut", m_micAudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("OutputChannelNumbers", m_OutputChannelNumbers, "1");
  bindParameter("MixGains", m_MixGains, "0.5,0.5");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Mixer::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Mixer::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Mixer::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  BufferClr();
  RTC_DEBUG(("onActivated finish"));
}


void Mixer::BufferClr(void)
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

void Mixer::RcvInBuffer(RTC::TimedOctetSeq data)
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
    RTC_DEBUG(("Input1:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_indata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

void Mixer::RcvOutBuffer(RTC::TimedOctetSeq data)
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
    RTC_DEBUG(("Input2:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_outdata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvOutBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvOutBuffer finish"));
  return;
}

RTC::ReturnCode_t Mixer::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Mixer::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Mixer::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if( ( m_indata.size() > BUFFER_MAX ) || ( m_outdata.size() > BUFFER_MAX ) ) {
    RTC_INFO(("One of buffers exceeded the maximum value. Start clear buffers."));
    BufferClr();
  }
  if (!m_indata.empty() && !m_outdata.empty()) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int num = (m_indata.size() < m_outdata.size()) ? m_indata.size() : m_outdata.size();
    double *buffer;
    buffer = new double[num * m_OutputChannelNumbers];
    int cnt = 0;
    for (int i = 0; i < num; i++) {
      for (int j = 0; j < m_OutputChannelNumbers; j++) {
        if (m_MixGains.size() > 2*j+1)
          buffer[cnt++] = m_MixGains[2*j] * (double)m_indata.front() + m_MixGains[2*j+1] * (double)m_outdata.front();
        else
          buffer[cnt++] = 0.0;
      }
      m_indata.pop_front();
      m_outdata.pop_front();
    }
    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
    m_micAudioDataOut.data.length(num * m_OutputChannelNumbers * 2);  //!< set outport data length
    for ( int i = 0; i < num * m_OutputChannelNumbers; i++ ) {
      short val = buffer[i];
      m_micAudioDataOut.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_micAudioDataOut.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    delete [] buffer;
    setTimestamp( m_micAudioDataOut );
    m_micAudioDataOutOut.write();
    RTC_DEBUG(("onExecute:writing %d samples", m_micAudioDataOut.data.length() / 2));
  } else {
    RTC_DEBUG(("onExecute:either or both the buffer is empty"));
  }
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Mixer::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Mixer::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void MixerInit(RTC::Manager* manager)
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
			     RTC::Create<Mixer>,
			     RTC::Delete<Mixer>);
  }
};
