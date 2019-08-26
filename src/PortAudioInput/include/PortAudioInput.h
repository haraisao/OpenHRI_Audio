// -*- C++ -*-
/*!
 * @file  PortAudioInput.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _PortAudioInput_H_
#define _PortAudioInput_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/
#include <portaudio.h>
#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#elif defined(__linux)
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#endif
/*
  Data Types
*/
#include <rtm/idl/BasicDataTypeSkel.h>

/*
  for RTC
*/
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/SystemLogger.h>
#include <coil/Mutex.h>

#include <rtm/ConnectorListener.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

#define FRAMES_PER_BUFFER (512)
#define HMIXER_NUM_MAX	(10)

using namespace RTC;

/*!
 * @class PortAudioInput
 * @brief Periodic Console Out Component
 *
 */
class PortAudioInput
  : public RTC::DataFlowComponentBase
{
 public:
  void SetGain(long data);
  unsigned long m_totalframes;      //!< Max frames to buffer size
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  PortAudioInput(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~PortAudioInput();

  // <rtc-template block="public_attribute">

  // </rtc-template>

  // <rtc-template block="public_operation">

  // </rtc-template>

   /**
     Actions
  */
   
   virtual RTC::ReturnCode_t onInitialize();

   virtual RTC::ReturnCode_t onFinalize();

   /* virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id); */

   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

   /* virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id); */

   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

   /* virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onAction(RTC::UniqueId ec_id); */

   /* virtual RTC::ReturnCode_t onModeChanged(RTC::UniqueId ec_id); */



 protected:
  // <rtc-template block="protected_attribute">

  // </rtc-template>

  // <rtc-template block="protected_operation">

  // </rtc-template>

  // DataPort declaration
  // <rtc-template block="dataport_declare">

  RTC::TimedLong m_GainDataIn;
  InPort<RTC::TimedLong> m_GainDataInIn;

  RTC::TimedOctetSeq m_AudioDataOut;
  OutPort<RTC::TimedOctetSeq> m_AudioDataOutOut;



  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">

  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">

  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">

  // </rtc-template>

 private:
  PaSampleFormat getFormat(std::string str);
  PaStream *m_stream;          //!< PortAudio Stream Object
  PaError m_err;               //!< Error codes returned by PortAudio functions
  PaSampleFormat m_format;     //!< Sample format type (paInt16, paInt24, paInt8)
  std::string m_gainstr;
  float m_volume;
  bool is_active;
  bool syncflg;

#ifdef HAVE_LIBPORTMIXER
  void *m_mixer;
#else
#  if defined(_WIN32)
  bool InitMixer(void);
  void CloseMixer(void);
  bool GetMicrophoneLevel(DWORD* dwLevel);
  bool SetMicrophoneLevel(DWORD dwLevel);
  HMIXER m_mixer[HMIXER_NUM_MAX];
  BOOL m_barrOpened[HMIXER_NUM_MAX];
  HMIXER m_mixerMic;
  int m_mixerNums;
  bool m_isOpen;
  DWORD m_dwCntlID,m_dwMin,m_dwMax;
#  elif defined(__linux)
  int m_device,m_fd;
#  endif
#endif
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_InputSampleRate;
  int m_InputChannelNumbers;
  std::string m_InputSampleByte;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class PortAudioInDataListener
  : public ConnectorDataListenerT<RTC::TimedLong>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  PortAudioInDataListener(const char* name, PortAudioInput *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~PortAudioInDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedLong& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
      m_obj->SetGain(data.data);
    }
    return NO_CHANGE;
  };

  PortAudioInput *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief PortAudioInput initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void PortAudioInputInit(RTC::Manager* manager);
};


#endif // _PortAudioInput_H_
