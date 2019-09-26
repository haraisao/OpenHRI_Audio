// -*- C++ -*-
/*!
 * @file  NoiseReduction.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "NoiseReduction.h"

// use speex's internal fft functions
extern "C" {
  void *spx_fft_init(int size);
  void spx_fft_destroy(void *table);
  void spx_fft_float(void *table, float *in, float *out);
  void spx_ifft_float(void *table, float *in, float *out);
}

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "NoiseReduction",
    "type_name",         "NoiseReduction",
    "description",       "Noise Reduction",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "component_type",    "STATIC",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    
    "conf.default.SampleRate", "16000",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__type__.SampleRate", "int",
    "conf.__description__.SampleRate", "入力音声データのサンプリング周波数(Hz)",
    "conf.default.CenterFrequency", "500",
    "conf.__constraints__.CenterFrequency", "x >= 1",
    "conf.__type__.CenterFrequency", "int",
    "conf.__description__.CenterFrequency", "プリエンファシスモード及びディエンファシスモードでの中心周波数(Hz)",
    "conf.default.FrequencyMax", "700",
    "conf.__constraints__.FrequencyMax", "x >= 1",
    "conf.__type__.FrequencyMax", "int",
    "conf.__description__.FrequencyMax", "バンドパスフィルタモードでの最大周波数(Hz)",
    "conf.default.FrequencyMin", "300",
    "conf.__constraints__.FrequencyMin", "x >= 1",
    "conf.__type__.FrequencyMin", "int",
    "conf.__description__.FrequencyMin", "バンドパスフィルタモードでの最小周波数(Hz)",
    "conf.default.Power", "1",
    "conf.__constraints__.Power", "x >= 1",
    "conf.__type__.Power", "int",
    "conf.__description__.Power", "強調倍率",
    "conf.default.ModeSelect", "premp",
    "conf.__constraints__.ModeSelect", "premp,deemp,bpf",
    "conf.__type__.ModeSelect", "string",
    "conf.__description__.ModeSelect", "フィルタモード選択 「premp」=プリエンファシスモード、「deemp」=ディエンファシスモード、「bpf」=バンドパスフィルタモード ",

    ""
  };
// </rtc-template>


//! Band-pass filter
void NoiseReduction::bpf(float *fdata)
{
  float df = ((float)m_SampleRate) / ((float)WINLEN);

  if (m_FrequencyMax < m_FrequencyMin) return;

  for (int i = 0; i < WINLEN; i++) {
    if (((i * df) > (float)m_FrequencyMin) && ((i * df) < (float)m_FrequencyMax)) {
      fdata[i*2+0] *= m_Power;
      fdata[i*2+1] *= m_Power;
    }
  }

}

//! pre-emphasis
void NoiseReduction::pre_emp(float *fdata)
{
  if (m_CenterFrequency > m_SampleRate) return;
  for (int i = 0; i < WINLEN; i++) {
    fdata[i*2+0] *= gauss[i] * m_Power;
    fdata[i*2+1] *= gauss[i] * m_Power;
  }
}

//! de-emphasis
void NoiseReduction::de_emp(float *fdata)
{
  if (m_CenterFrequency > m_SampleRate) return;
  for ( int i = 0; i < WINLEN; i++ ) {
    fdata[i*2+0] /= gauss[i] * m_Power;
    fdata[i*2+1] /= gauss[i] * m_Power;
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
NoiseReduction::NoiseReduction(RTC::Manager* manager)
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
NoiseReduction::~NoiseReduction()
{
}

RTC::ReturnCode_t NoiseReduction::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("NoiseReduction : Noise Reduction"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("SampleRate", m_SampleRate, "16000");
  bindParameter("CenterFrequency", m_CenterFrequency, "500");
  bindParameter("FrequencyMax", m_FrequencyMax, "700");
  bindParameter("FrequencyMin", m_FrequencyMin, "300");
  bindParameter("Power", m_Power, "1");
  bindParameter("ModeSelect", m_ModeSelect, "premp");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t NoiseReduction::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t NoiseReduction::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t NoiseReduction::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  m_mutex.lock();
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();

  window = CreateWindowFloat(WINLEN, Hamming);
  gauss = CreateGaussWindowFloat(WINLEN, m_SampleRate, m_CenterFrequency);
  fft = spx_fft_init(WINLEN);
  for (int i = 0; i < WINLEN/2; i++) {
    m_outbuffer.push_back(0);
  }
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void NoiseReduction::RcvInBuffer(RTC::TimedOctetSeq data)
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


RTC::ReturnCode_t NoiseReduction::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t NoiseReduction::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t NoiseReduction::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if (m_inbuffer.size() >= WINLEN) {
    int i;
    float data[WINLEN];
    float fdata[WINLEN*2];
    std::list<short>::iterator pbuffer;

    // sliding window with half overlap
    for (i = 0; i < WINLEN/2; i++) {
      data[i] = (float)m_inbuffer.front();
      m_inbuffer.pop_front();
    }
    pbuffer = m_inbuffer.begin();
    for (i = WINLEN/2; i < WINLEN; i++) {
      data[i] = (float)*pbuffer;
      pbuffer++;
    }

    // apply window function
    ApplyWindowFloat(WINLEN, window, data);

    // convert to frequency domain
    spx_fft_float(fft, data, fdata);

    // sampling frequency 16000Hz, number 1024, resolution 16000/1024=15.625
    // voice frequency male：300～550Hz,female：400～700Hz
    if ( m_ModeSelect == "premp" ) {
      //! pre-emphasis
      pre_emp(fdata);
    } else if ( m_ModeSelect == "deemp" ) {
      //! de-emphasis
      de_emp(fdata);
    } else if ( m_ModeSelect == "bpf" ) {
      //! band-pass
      bpf(fdata);
    }

    // return back to time domain
    spx_ifft_float(fft, fdata, data);
    ApplyWindowFloat(WINLEN, window, data);

    // sum it up (half overlap)
    pbuffer = m_outbuffer.begin();
    for (i = 0; i < WINLEN/2; i++) {
      *pbuffer += (short)(data[i]);
      pbuffer++;
    }
    for (i = WINLEN/2; i < WINLEN; i++) {
      m_outbuffer.push_back((short)(data[i]));
    }
    // output the resulting signal
    m_AudioDataOut.data.length(WINLEN);
    short val;
    for (i = 0; i < WINLEN/2; i++) {
      val = m_outbuffer.front();
      m_AudioDataOut.data[i*2] = (unsigned char)(val & 0x00ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
      m_outbuffer.pop_front();
    }
    setTimestamp(m_AudioDataOut);
    m_AudioDataOutOut.write();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t NoiseReduction::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t NoiseReduction::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void NoiseReductionInit(RTC::Manager* manager)
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
			     RTC::Create<NoiseReduction>,
			     RTC::Delete<NoiseReduction>);
  }
};
