// -*- C++ -*-
/*!
 * @file  Mixer.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _Mixer_H_
#define _Mixer_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/

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
#define BUFFER_MAX 262144

/*!
 * @class Mixer
 * @brief Periodic Console Out Component
 *
 */
class Mixer
  : public RTC::DataFlowComponentBase
{
 public:
  void RcvInBuffer(RTC::TimedOctetSeq data);
  void RcvOutBuffer(RTC::TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Mixer(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Mixer();

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

  RTC::TimedOctetSeq m_micAudioDataIn;
  InPort<RTC::TimedOctetSeq> m_micAudioDataInIn;

  RTC::TimedOctetSeq m_micReferenceAudioDataIn;
  InPort<RTC::TimedOctetSeq> m_micReferenceAudioDataInIn;

  RTC::TimedOctetSeq m_micAudioDataOut;
  OutPort<RTC::TimedOctetSeq> m_micAudioDataOutOut;



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
  void BufferClr(void);
  bool is_active;

  std::list<short> m_indata; //!< receive buffer queue
  std::list<short> m_outdata; //!< receive buffer queue
  int m_err;  
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_OutputChannelNumbers;
  std::vector<double> m_MixGains;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class MicAudioDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  MicAudioDataListener(const char* name, Mixer *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~MicAudioDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
     m_obj->RcvInBuffer(data);
    }
    return NO_CHANGE;
  };

  Mixer *m_obj;
  std::string m_name;
};


/*!
 * @class DataListener
 * @brief
 */
class MicReferenceDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  MicReferenceDataListener(const char* name, Mixer *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~MicReferenceDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
     m_obj->RcvOutBuffer(data);
    }
    return NO_CHANGE;
  };

  Mixer *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief Mixer initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void MixerInit(RTC::Manager* manager);
};


#endif // _Mixer_H_
