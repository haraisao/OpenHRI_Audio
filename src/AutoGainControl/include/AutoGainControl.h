// -*- C++ -*-
/*!
 * @file  AutoGainControl.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _AutoGainControl_H_
#define _AutoGainControl_H_

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

/*!
 * @class AutoGainControl
 * @brief Periodic Console Out Component
 *
 */
class AutoGainControl
  : public RTC::DataFlowComponentBase
{
 public:
  void RcvBuffer(RTC::TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  AutoGainControl(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~AutoGainControl();

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

  RTC::TimedLong m_AudioDataOut;
  OutPort<RTC::TimedLong> m_AudioDataOutOut;



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
  bool is_active;
  std::list<unsigned char> m_data; //!< receive buffer queue
  double m_gain, m_agc, m_div1, m_div2;
  unsigned long m_totalframes; //!< Max frames to buffer size
  int m_err;
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  std::string m_GainCtlSampleByte;
  std::string m_InputSampleByte;
  int m_AutomaticGainControl;
  int m_InputChannelNumbers;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class AudioDataInDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  AudioDataInDataListener(const char* name, AutoGainControl *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~AudioDataInDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
      AutoGainControl *p = (AutoGainControl *)m_obj;
      p->RcvBuffer(data);
    }
    return NO_CHANGE;
  };

  AutoGainControl *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief AutoGainControl initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void AutoGainControlInit(RTC::Manager* manager);
};


#endif // _AutoGainControl_H_
