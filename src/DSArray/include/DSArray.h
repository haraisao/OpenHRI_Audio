// -*- C++ -*-
/*!
 * @file  DSArray.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _DSArray_H_
#define _DSArray_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/
#include "windowing.h"
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

struct mic_info {
  double x,y,z;
  double xy_rad,yz_rad;
};

struct delay_rank {
  int mic_num,trigger_cnt;
};

using namespace RTC;
#define SONIC 340.29
#define WINLEN 2048
#define SEARCHMAX 100

/*!
 * @class DSArray
 * @brief Periodic Console Out Component
 *
 */
class DSArray
  : public RTC::DataFlowComponentBase
{
 public:
  void RcvBuffer(RTC::TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  DSArray(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~DSArray();

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

  RTC::TimedOctetSeq m_mic;
  InPort<RTC::TimedOctetSeq> m_micIn;

  RTC::TimedDouble m_result;
  OutPort<RTC::TimedDouble> m_resultOut;



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
  std::list<short> m_data; //!< receive buffer queue

  mic_info* m_micinfo;
  void *fft;
  float *window;
  bool is_active;
  bool m_horizon;
  // </rtc-template>

  // <rtc-template block="private_operation">
  int ccf(short *base, short *data);
  int CrossCorrelation(short *base, short *data);
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_SampleRate;
  int m_ChannelNumbers;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class MicDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  MicDataListener(const char* name, DSArray *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~MicDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
      m_obj->RcvBuffer(data);
    }
    return NO_CHANGE;
  };

  DSArray *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief DSArray initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void DSArrayInit(RTC::Manager* manager);
};


#endif // _DSArray_H_
