// -*- C++ -*-
/*!
 * @file  SamplingRateConverter.h
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#ifndef _SamplingRateConverter_H_
#define _SamplingRateConverter_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>

/*
 insert include files for 3rd party libs
*/
#include <libresample.h>

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
 * @class SamplingRateConverter
 * @brief Periodic Console Out Component
 *
 */
class SamplingRateConverter
  : public RTC::DataFlowComponentBase
{
 public:
   void RcvInBuffer(RTC::TimedOctetSeq data);

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  SamplingRateConverter(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~SamplingRateConverter();

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
  float *InputDataSetFloat(int *length);
  long *InputDataSet(int *length);
  void OutputDataWriteFloat(float *data, int length);
  void OutputDataWrite(long *data, int length);
  double DeprhGain(void);
  bool is_active;

  std::list<unsigned char> m_queue; //!< receive buffer queue
  void *m_handle;
  double m_ratio;
  int m_in_format;          //!< config parameter(InputSampleByte)
  int m_out_format;         //!< config parameter(OutputSampleByte)
  int m_in_channels;                   //!< config parameter(InputChannelNumbers)
  int m_out_channels;                  //!< config parameter(OutputChannelNumbers)

  int m_err;                   //!< Error codes returned by PulseAudio functions

  // <rtc-template block="private_attribute">
  coil::Mutex m_mutex;
  
  int m_InputSampleRate;
  std::string m_InputBitDepth;
  int m_OutputSampleRate;
  std::string m_OutputBitDepth;


  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};


/*!
 * @class DataListener
 * @brief
 */
class AudioInDataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
  USE_CONNLISTENER_STATUS;
public:
  /*!
   * @brief constructor
   */
  AudioInDataListener(const char* name, SamplingRateConverter *data) : m_name(name), m_obj(data){};

  /*!
   * @brief destructor
   */
  virtual ~AudioInDataListener(){};

  virtual ReturnCode operator()( ConnectorInfo& info,
                                 RTC::TimedOctetSeq& data){
    if ( m_name == "ON_BUFFER_WRITE" ) {
     /* onBufferWrite */
     m_obj->RcvInBuffer(data);
    }
    return NO_CHANGE;
  };

  SamplingRateConverter *m_obj;
  std::string m_name;
};



extern "C"
{
  /*!
   * @brief SamplingRateConverter initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void SamplingRateConverterInit(RTC::Manager* manager);
};


#endif // _SamplingRateConverter_H_
