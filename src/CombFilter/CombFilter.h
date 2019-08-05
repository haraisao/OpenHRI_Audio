// -*- C++ -*-
/*!
 * @file  CombFilter.h
 * @author Yosuke Matsusaka <yosuke.matsusaka@aist.go.jp> and OpenHRI development team
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date  $Date$
 *
 * $Id$
 */

#ifndef COMBFILTER_H
#define COMBFILTER_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <list>
#include <fstream>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/SystemLogger.h>

#include <coil/Mutex.h>

#include "OpenHRI.h"

//#include "stdafx.h"

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;
#define PI 3.14159265
#define MAXSIZE 1024
#define HALFSIZE (MAXSIZE/2)
#define NOISESAMPLE 10

/*!
 * @class CombFilter
 * @brief Periodic Console Out Component
 *
 */
class CombFilter
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief receive inport buffer
   */
  void RcvInBuffer(TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CombFilter(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CombFilter();

  // <rtc-template block="public_attribute">

  // </rtc-template>

  // <rtc-template block="public_operation">

  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry()
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);

  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);

 protected:
  // <rtc-template block="protected_attribute">

  // </rtc-template>

  // <rtc-template block="protected_operation">

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  TimedOctetSeq m_nin;
  DescriptableInPort<TimedOctetSeq> m_ninIn;

  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  TimedOctetSeq m_fout;
  DescriptableOutPort<TimedOctetSeq> m_foutOut;

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
  void dftfnc(short*, int);
  bool is_active;
  coil::Mutex m_mutex;
  std::list<short> m_queue; //!< receive buffer queue
//  double m_noise[MAXSIZE][2];
//  short m_outbuffer[HALFSIZE];
  std::list<short> m_dbuff;
  int m_rate;                       //!< config parameter(InputSampleRate)
  int m_channels;                   //!< config parameter(InputChannelNumbers)
  int m_freq;                   //!< config parameter()
  int m_gain;
//  int m_noise_end;
//  int m_err;                   //!< Error codes returned by PulseAudio functions
  // <rtc-template block="private_attribute">

  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};

/*!
 * @class DataListener
 * @brief
 */
class DataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   *
   * @param name DataListener event name
   * @param data CombFilter Object
   */
  DataListener(const char* name, CombFilter *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~DataListener(){};
  virtual ReturnCode operator()(_CONST ConnectorInfo& info,
                                _CONST TimedOctetSeq& data){

    if ( m_name == "ON_BUFFER_WRITE" ) {
      m_obj->RcvInBuffer(data);
    }
    return NO_CHANGE;
  };

  CombFilter *m_obj;
  std::string m_name;
};

#if 0
/*!
 * @class CombFilterManager
 * @brief
 *
 */
class CombFilterManager
{
public:
  /*!
   * @brief constructor
   */
  CombFilterManager();

  /*!
   * @brief destructor
   */
  ~CombFilterManager();

  /*!
   * @brief CombFilterManager start
   */
  void start();
private:
  RTC::Manager* m_manager;  //!< Maneger Object

  /*!
   * @brief CombFilterManager initialize
   *
   * @param manager Maneger Object
   */
  static void ModuleInit(RTC::Manager* manager);
};
#endif
extern "C"
{
  /*!
   * @brief CombFilter initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void CombFilterInit(RTC::Manager* manager);
};


#endif // COMBFILTER_H
