// -*- C++ -*-
/*!
 * @file  WavPlayer.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _WavPlayer_H_
#define _WavPlayer_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/
#include <sndfile.h>
#if defined(__linux)
#include <gtkmm.h>
#elif defined(_WIN32)
#include <windows.h>
#include <CommDlg.h>
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

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class WavPlayer
 * @brief Periodic Console Out Component
 *
 */
class WavPlayer
  : public RTC::DataFlowComponentBase
{
 public:
  unsigned long m_totalframes;      //!< Max frames to buffer size
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  WavPlayer(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~WavPlayer();

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
  coil::TimeValue m_timer;
  double m_pbuff;
  int m_cnt;
  bool m_flg;

  SNDFILE* sfr;
  SF_INFO sfinfo;
  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_OutputSampleRate;
  int m_ChannelNumbers;
  std::string m_FileName;
  int m_Delay;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};



extern "C"
{
  /*!
   * @brief WavPlayer initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void WavPlayerInit(RTC::Manager* manager, char * wave_file_name);
};


#endif // _WavPlayer_H_
