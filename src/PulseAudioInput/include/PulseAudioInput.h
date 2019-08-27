// -*- C++ -*-
/*!
 * @file  PulseAudioInput.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _PulseAudioInput_H_
#define _PulseAudioInput_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <pulse/context.h>
#include <pulse/version.h>
#include <pulse/volume.h>
#include <pulse/channelmap.h>

/*
  Data Types
*/
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

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

using namespace RTC;

/*!
 * @class PulseAudioInput
 * @brief Periodic Console Out Component
 *
 */
class PulseAudioInput
  : public RTC::DataFlowComponentBase
{
 public:
  void RcvBuffer(RTC::TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  PulseAudioInput(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~PulseAudioInput();

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
  pa_sample_format getFormat(std::string str);

  bool is_active;
  bool syncflg;
  pa_cvolume *mp_vol;
  pa_sample_spec m_spec;       //!< sample spec (sample rate, format, channels)
  pa_simple *m_simple;         //!< PulseAudio Simple Connection Object
  std::list<unsigned char> m_data; //!< receive buffer queue
  unsigned long m_totalframes; //!< Max frames to buffer size
  int m_err;    
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
class GainDataDataListener
  : public ConnectorDataListenerT<RTC::TimedLong>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  GainDataDataListener(const char* name, PulseAudioInput *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~GainDataDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedLong& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
      m_obj->SetGain(data.data);
    }
    return NO_CHANGE;
  };

  PulseAudioInput *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief PulseAudioInput initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void PulseAudioInputInit(RTC::Manager* manager);
};


#endif // _PulseAudioInput_H_
