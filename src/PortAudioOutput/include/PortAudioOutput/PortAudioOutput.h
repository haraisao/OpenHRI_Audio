// -*- C++ -*-
/*!
 * @file  PortAudioOutput.h
 * @author isao-hara@aist.go.jp
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _PortAudioOutput_H_
#define _PortAudioOutput_H_

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
/*
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

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>
#define FRAMES_PER_BUFFER (512)
using namespace RTC;

/*!
 * @class PortAudioOutput
 * @brief Periodic Console Out Component
 *
 */
class PortAudioOutput
  : public RTC::DataFlowComponentBase
{
 public:
  void RcvBuffer(TimedOctetSeq data);
  unsigned long m_totalframes;      //!< Max frames to buffer size
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  PortAudioOutput(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~PortAudioOutput();

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

  RTC::TimedOctetSeq m_AudioDataIn;
  InPort<RTC::TimedOctetSeq> m_AudioDataInIn;

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

  bool is_active;
  PaStream *m_stream;               //!< PortAudio Stream Object
  PaError m_err;                    //!< Error codes returned by PortAudio functions
  std::list<char> m_data;           //!< receive buffer queue
  PaSampleFormat m_format;          //!< Sample format type (paInt16, paInt24, paInt8)
  int m_delay;
  bool m_waitflg;
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_OutputSampleRate;
  int m_OutputChannelNumbers;
  std::string m_InputSampleByte;
  int m_DelayCount;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class PortAudioOutputDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  PortAudioOutputDataListener(const char* name, PortAudioOutput *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~PortAudioOutputDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
      m_obj->RcvBuffer(data);
    }
    return NO_CHANGE;
  };

  PortAudioOutput *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief PortAudioOutput initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void PortAudioOutputInit(RTC::Manager* manager);
};


#endif // _PortAudioOutput_H_
